# Sources
add_library(zerolib
    Server.h
    Exception.h
    HttpMethod.h
    HttpRequest.h
    HttpRequestParser.h
    HttpResponse.h

    Server.cpp
    Exception.cpp
    HttpRequest.cpp
    HttpRequestParser.cpp
    HttpResponse.cpp
)

# Dependencies
spdlog_enable_warnings(zerolib)

target_include_directories(zerolib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zerolib PUBLIC spdlog::spdlog http_parser PRIVATE cpp_redis spdlog::spdlog magic_enum)
