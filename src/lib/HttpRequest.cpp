#include "HttpRequest.h"

#include <utility>

namespace zero {
    HttpRequest::HttpRequest(HttpMethod method, std::string&& url, std::string&& body)
        : Method(method)
        , Url(std::move(url))
        , Body(std::move(body))
    {
    }

    HttpMethod HttpRequest::GetMethod() const {
        return Method;
    }

    const std::string& HttpRequest::GetUrl() const {
        return Url;
    }

    const std::string& HttpRequest::GetBody() const {
        return Body;
    }
}
