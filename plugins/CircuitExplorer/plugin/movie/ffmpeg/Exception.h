/* Copyright (c) 2021, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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