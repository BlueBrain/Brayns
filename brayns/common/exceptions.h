#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

#include <brayns/common/log.h>

namespace brayns
{

class Exception : public std::exception
{
public:
    Exception( const std::string& message ) : message_(message) {}

protected:
    std::string message_;
};

class BadParameter : public Exception
{
public:
    BadParameter( const std::string& message ) : Exception(message)
    {
        BRAYNS_ERROR << "<" << message_ << "> is a bad parameter" << std::endl;
    }
};

}
#endif // EXCEPTIONS_H
