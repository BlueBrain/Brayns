#pragma once

#include <stdexcept>

class MovieMakerException : public std::runtime_error
{
public:
    MovieMakerException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};