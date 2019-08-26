#pragma once

#include <memory>

#include <spdlog/spdlog.h>

namespace zero {
    void TerminateHandler();

    class Server {
    public:
        Server(int port = 8000);

        int RunLoop();

        void HandleClient(int clientSocket);

    private:
        void InstallTerminateHandler();

        void CreateListeningSocket();

        std::shared_ptr<spdlog::logger> Logger;

        int Port;
        int SocketFd = -1;
    };
}
