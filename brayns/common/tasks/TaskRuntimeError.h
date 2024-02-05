/* Copyright 2015-2024 Blue Brain Project/EPFL
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

namespace brayns
{
/**
 * An exception type that shall be thrown at any point during the task execution
 * to provide useful errors for the user.
 */
class TaskRuntimeError : public std::runtime_error
{
public:
    TaskRuntimeError(const std::string& message, const int code_ = -1,
                     const std::string& data_ = "")
        : std::runtime_error(message)
        , code(code_)
        , data(data_)
    {
    }

    const int code;
    const std::string data;
};
} // namespace brayns
