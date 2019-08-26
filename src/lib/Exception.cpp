#include "Exception.h"

#include <sstream>
#include <cxxabi.h>
#include <string.h>

#include <magic_enum.hpp>

namespace zero {

    Exception::Exception(const std::string& message, ErrorCode code)
        : message(message)
        , code(code)
    {
    }

    std::string Exception::What() const {
        std::stringstream result;
        result << magic_enum::enum_name(code) << ": " << message;
        return result.str();
    }

    std::string GetCurrentExceptionMessage() {
        try {
            throw;
        } catch (const Exception& e) {
            return e.What();
        } catch (const std::exception& e) {
            return Exception(e.what(), ErrorCode::StdException).What();
        } catch (...) {
            int status;
            const char* type_name = abi::__cxa_current_exception_type()->name();
            char* demangled_type_name = abi::__cxa_demangle(type_name, 0, 0, &status);

            std::string message;
            if (demangled_type_name) {
                try {
                    message = demangled_type_name;
                } catch(...) {
                    ::free(demangled_type_name);
                    throw;
                }
                ::free(demangled_type_name);
            } else {
                message = type_name;
            }

            return Exception("No message. Exception type: '" + message + "'", ErrorCode::UnknownException).What();
        }
    }

    std::string ErrnoToString(int error_code) {
        std::stringstream result;
        result << "Errno: " << error_code << ", ";

        const size_t buf_size = 128;
        char buf[buf_size];
#ifndef _GNU_SOURCE
        int status = ::strerror_r(error_code, buf, buf_size);

#ifdef __APPLE__
        if (status != 0 && status != EINVAL) {
#else
        if (status != 0) {
#endif
            result << "Unknown error";
        } else {
            result << buf;
        }

#else
        result << ::strerror_r(error_code, buf, sizeof(buf));
#endif

        return result.str();
    }

    void ThrowFromErrno(const std::string& message, ErrorCode code, int errno_code) {
        throw Exception(message + ", " + ErrnoToString(errno_code), code);
        return;
    }
}
