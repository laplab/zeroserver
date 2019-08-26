#include "HttpRequestParser.h"

namespace zero {
    namespace {
        HttpMethod ConvertHttpMethod(int method) {
            switch (method) {
                case HTTP_DELETE:
                    return HttpMethod::Delete;
                case HTTP_GET:
                    return HttpMethod::Get;
                case HTTP_HEAD:
                    return HttpMethod::Head;
                case HTTP_POST:
                    return HttpMethod::Post;
                case HTTP_PUT:
                   return HttpMethod::Put;
                default:
                  return HttpMethod::Get;
            }
        }
    }

    HttpRequestParser::HttpRequestParser() {
        auto noop_callback = [](http_parser*) { return 0; };
        auto noop_data_callback = [](http_parser*, const char*, size_t) { return 0; };

        // TODO: add headers parsing
        // TODO: add body parsing
        // TODO: add overflow checks for body, headers and URL
        Settings.on_message_begin = noop_callback;
        Settings.on_url = [](http_parser* parser, const char* start, size_t length) {
            auto state = reinterpret_cast<HttpRequestState*>(parser->data);
            state->Url << std::string_view(start, length);
            state->UrlTotalLength += length;
            return 0;
        };
        Settings.on_status = noop_data_callback;
        Settings.on_header_field = noop_data_callback;
        Settings.on_header_value = noop_data_callback;
        Settings.on_headers_complete = [](http_parser* parser) {
            auto state = reinterpret_cast<HttpRequestState*>(parser->data);
            state->Method = ConvertHttpMethod(parser->method);
            state->HeadersComplete = true;
            return 0;
        };
        Settings.on_body = noop_data_callback;
        Settings.on_message_complete = [](http_parser* parser) {
            auto state = reinterpret_cast<HttpRequestState*>(parser->data);
            state->MessageComplete = true;
            return 0;
        };
        Settings.on_chunk_header = noop_callback;
        Settings.on_chunk_complete = noop_callback;

        http_parser_init(&Parser, HTTP_REQUEST);
        Parser.data = reinterpret_cast<void *>(&State);
    }

    HttpParseResult HttpRequestParser::Put(const char* buffer, size_t buffer_len) {
        size_t count_parsed = http_parser_execute(&Parser, &Settings, buffer, buffer_len);

        if (count_parsed != buffer_len) {
            return HttpParseResult::Error;
        }

        if (State.MessageComplete) {
            return HttpParseResult::Complete;
        }

        return HttpParseResult::NeedMore;
    }

    std::optional<HttpRequest> HttpRequestParser::TryGetRequest() const {
        if (!State.Method.has_value() || State.UrlTotalLength == 0) {
            return {};
        }

        // FIXME: URL and Body are copied here
        return HttpRequest(*State.Method, State.Url.str(), State.Body.str());
    }
}
