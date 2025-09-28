#pragma once

#include <string>

namespace zaphod
{
    struct Result
    {
        enum class Code
        {
            SUCCESS,
            FAILURE,
            NOT_INITIALIZED,
            ALREADY_INITIALIZED,
            INVALID_ARGUMENT,
            OUT_OF_MEMORY,
            IO_ERROR,
            TIMEOUT,
            UNSUPPORTED,
            UNKNOWN
        };

        Result(Code code = Code::SUCCESS, const std::string& message = "") : code(code), message(message) {}

        bool isSuccess() const { return code == Code::SUCCESS; }
        bool isFailure() const { return !isSuccess(); }

        Code        code;
        std::string message;
    };
}