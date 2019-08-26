#include "HttpResponse.h"

#include <sstream>
#include <unistd.h>

namespace zero {
    namespace {
        std::string ContentTypeToString(HttpContentType type) {
            switch (type) {
                case HttpContentType::Text:
                    return "text/plain";
            }
        }

        std::string StatusCodeToString(HttpStatusCode code) {
            switch (code) {
                case HttpStatusCode::Ok:
                    return "200 OK";
            }
        }
    }

    void HttpResponse::AddHeader(const std::string& name, const std::string& value) {
        Headers[name] = value;
    }

    HttpSendResult HttpResponse::SendHeaders(int fd) {
        AddHeader("Content-Type", ContentTypeToString(GetContentType()));
        AddHeader("Content-Length", std::to_string(GetContentLength()));

        std::stringstream headers;
        headers << "HTTP/1.0 " << StatusCodeToString(GetStatusCode()) << "\r\n";
        for (const auto& item : Headers) {
            headers << item.first << ": " << item.second << "\r\n";
        }
        // headers finished
        headers << "\r\n";

        const std::string result = headers.str();
        const char* buf = result.c_str();
        const size_t length = result.length(); // do no write finishing \0
        size_t offset = 0;
        ssize_t written = 0;

        while((written = ::write(fd, buf + offset, length - offset))) {
            if (written == -1) {
                return HttpSendResult::Error;
            }

            offset += written;
        }

        if (offset != length) {
            return HttpSendResult::Error;
        }

        return HttpSendResult::Complete;
    }
}
