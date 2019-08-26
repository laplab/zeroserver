#include "Server.h"

#include <iostream>
#include <optional>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <magic_enum.hpp>

#include <Exception.h>
#include <HttpRequestParser.h>
#include <HttpResponse.h>

namespace zero {
    void TerminateHandler() {
        static thread_local bool terminating = false;
        if (terminating) {
            ::abort();
        }

        terminating = true;

        static auto logger = spdlog::stderr_color_mt("TerminateHandler");

        if (std::current_exception()) {
            logger->critical("Terminate called for uncaught exception. " + GetCurrentExceptionMessage());
        } else {
            logger->critical("Terminate called without active exception");
        }

        ::abort();
    }

    Server::Server(int port)
        : Logger(spdlog::stderr_color_mt("Server"))
        , Port(port)
    {
        InstallTerminateHandler();
        CreateListeningSocket();
    }

    void Server::InstallTerminateHandler() {
        std::set_terminate(TerminateHandler);
    }

    int Server::RunLoop() {
        Logger->info("Ready for connections");

        struct sockaddr_in clientAddress;
        socklen_t clientLength = sizeof(clientAddress);

        while (true) {
            int clientSocket = ::accept(SocketFd, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientLength);
            if (clientSocket == -1) {
                ThrowFromErrno("Cannot accept client request", ErrorCode::CannotAccept, errno);
            }

            HandleClient(clientSocket);

            if (::close(clientSocket)) {
                ThrowFromErrno("Cannot close client socket", ErrorCode::CannotClose, errno);
            }
        }

        return 0;
    }

    void Server::HandleClient(int clientSocket) {
        constexpr size_t buf_size = 1024;
        char buf[buf_size];
        ssize_t buf_len = 0;

        HttpRequestParser parser;
        std::optional<HttpRequest> request;
        while ((buf_len = ::read(clientSocket, buf, buf_size))) {
            if (buf_len == -1) {
                ThrowFromErrno("Error while reading from client socket", ErrorCode::CannotRead, errno);
            }

            auto result = parser.Put(buf, buf_len);

            if (result == HttpParseResult::Error) {
                break;
            }

            if (result == HttpParseResult::Complete) {
                request = parser.TryGetRequest();
                break;
            }
        }

        if (!request.has_value()) {
            Logger->error("Failed to parse HTTP request");
            return;
        }

        std::stringstream log;
        log << magic_enum::enum_name(request->GetMethod()) << ": " << request->GetUrl();
        Logger->info(log.str());

        EmptyResponse response;
        if (response.SendHeaders(clientSocket) != HttpSendResult::Complete) {
            Logger->error("Cannot send headers to client");
        }

        if (response.SendContent(clientSocket) != HttpSendResult::Complete) {
            Logger->error("Cannot send content to client");
        }
    }

    void Server::CreateListeningSocket() {
        SocketFd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (SocketFd == -1) {
            ThrowFromErrno("Cannot create socket for server", ErrorCode::CannotSocket, errno);
        }

        const int enable = 1;
        if (::setsockopt(SocketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable))) {
            ThrowFromErrno("Cannot make socket with reusable address", ErrorCode::CannotSetsockopt, errno);
        }

        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons(Port);
        address.sin_addr.s_addr = htonl(INADDR_ANY);

        if (::bind(SocketFd, reinterpret_cast<struct sockaddr*>(&address), sizeof(address))) {
            ThrowFromErrno("Cannot bind socket to address", ErrorCode::CannotBind, errno);
        }

        if (::listen(SocketFd, 10)) {
            ThrowFromErrno("Cannot make listening socket", ErrorCode::CannotListen, errno);
        }
    }
}
