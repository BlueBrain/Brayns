/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

#include <brayns/common/log.h>

namespace brayns
{

class Exception: public std::exception
{
public:
    Exception(const std::string& message) : _message(message) {}

protected:
    std::string _message;
};

class BadParameter: public Exception
{
public:
    BadParameter(const std::string& message) : Exception(message)
    {
        BRAYNS_ERROR << "<" << _message << "> is a bad parameter" << std::endl;
    }
};

}
#endif // EXCEPTIONS_H
