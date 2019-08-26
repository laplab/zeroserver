#pragma once

#include <string>
#include <map>

namespace zero {
    enum class HttpStatusCode {
        Ok = 0,
    };

    enum class HttpContentType {
        Text = 0,
    };

    enum HttpSendResult {
        Complete = 0,
        Error = 1,
    };

    class HttpResponse {
    public:
        void AddHeader(const std::string& name, const std::string& value);

        HttpSendResult SendHeaders(int fd);

        virtual HttpSendResult SendContent(int fd) = 0;

        virtual HttpStatusCode GetStatusCode() = 0;

        virtual HttpContentType GetContentType() = 0;

        virtual size_t GetContentLength() = 0;

        virtual ~HttpResponse() = default;

    protected:
        std::map<std::string, std::string> Headers;
    };

    class EmptyResponse : public HttpResponse {
    public:
        HttpSendResult SendContent(int fd) override {
            return HttpSendResult::Complete;
        }

        HttpStatusCode GetStatusCode() override {
            return HttpStatusCode::Ok;
        }

        HttpContentType GetContentType() override {
            return HttpContentType::Text;
        }

        size_t GetContentLength() override {
            return 0;
        }
    };
}
