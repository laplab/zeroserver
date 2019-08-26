#pragma once

#include <string>

#include <HttpMethod.h>

namespace zero {
    class HttpRequest {
    public:
        HttpRequest(HttpMethod method, std::string&& url, std::string&& body);

        HttpMethod GetMethod() const;
        const std::string& GetUrl() const;
        const std::string& GetBody() const;

    private:
        HttpMethod Method;
        std::string Url;
        std::string Body;
    };
}
