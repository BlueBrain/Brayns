#pragma once

#include <stdexcept>
#include <string>

#include "Status.h"

namespace ffmpeg
{
class Exception : public std::runtime_error
{
public:
    Exception(const std::string& message)
        : std::runtime_error(message)
        , _message(message)
    {
    }

    Exception(const std::string& message, Status status)
        : std::runtime_error(message)
        , _message(message)
        , _status(status)
    {
    }

    const std::string& getMessage() const { return _message; }
    Status getStatus() const { return _status; }
    int getStatusCode() const { return _status.getCode(); }
    std::string getStatusDescription() const { return _status.toString(); }

private:
    std::string _message;
    Status _status;
};
} // namespace ffmpeg