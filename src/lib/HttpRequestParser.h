#pragma once

#include <optional>
#include <sstream>

#include <http_parser.h>

#include <HttpMethod.h>
#include <HttpRequest.h>

namespace zero {
    struct HttpRequestState {
        std::optional<HttpMethod> Method;
        std::stringstream Url;
        std::stringstream Body;
        bool HeadersComplete = false;
        bool MessageComplete = false;
        size_t UrlTotalLength = 0;
    };

    enum class HttpParseResult {
        NeedMore = 0,
        Error = 1,
        Complete = 2,
    };

    class HttpRequestParser {
    public:
        HttpRequestParser();

        HttpParseResult Put(const char* buffer, size_t buffer_len);

        std::optional<HttpRequest> TryGetRequest() const;

    private:
        http_parser_settings Settings;
        http_parser Parser;
        HttpRequestState State;
    };
}
