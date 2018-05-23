/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/common/tasks/Task.h>
#include <brayns/common/tasks/TaskRuntimeError.h>

namespace brayns
{
struct BinaryError
{
    std::vector<std::string> supportedTypes;
};

class BinaryTaskError : public TaskRuntimeError
{
public:
    BinaryTaskError(const std::string& message, const int code_,
                    const BinaryError& error_)
        : TaskRuntimeError(message, code_)
        , error(error_)
    {
    }

    const BinaryError error;
};

const TaskRuntimeError MISSING_PARAMS{"Missing params", -1731};

inline BinaryTaskError UNSUPPORTED_TYPE(const BinaryError& error)
{
    return {"Unsupported type", -1732, error};
}

const TaskRuntimeError INVALID_BINARY_RECEIVE{
    "Invalid binary received; no more files expected or "
    "current file is complete",
    -1733};

inline TaskRuntimeError LOADING_BINARY_FAILED(const std::string& error)
{
    return {error, -1734};
}
}
