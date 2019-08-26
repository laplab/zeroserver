#pragma once

#include <string>
#include <exception>

namespace zero {
    enum class ErrorCode {
        Ok = 0,
        Unknown = 1,
        CannotSocket = 2,
        CannotSetsockopt = 3,
        CannotBind = 4,
        CannotListen = 5,
        StdException = 6,
        UnknownException = 7,
        CannotAccept = 8,
        CannotClose = 9,
        CannotRead = 10,
        InvalidHttp = 11,
    };

    class Exception : public std::exception {
    public:
        Exception(const std::string& message, ErrorCode code);

        std::string What() const;

    private:
        std::string message;
        ErrorCode code;
    };

    std::string GetCurrentExceptionMessage();

    void ThrowFromErrno(const std::string& message, ErrorCode code, int errno_code);
}
