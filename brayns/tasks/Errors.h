/* Copyright 2015-2024 Blue Brain Project/EPFL
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
const auto ERROR_ID_MISSING_PARAMS = -1731;
const auto ERROR_ID_UNSUPPORTED_TYPE = -1732;
const auto ERROR_ID_INVALID_BINARY_RECEIVE = -1733;
const auto ERROR_ID_LOADING_BINARY_FAILED = -1734;

const TaskRuntimeError MISSING_PARAMS{"Missing params",
                                      ERROR_ID_MISSING_PARAMS};

const TaskRuntimeError UNSUPPORTED_TYPE{"Unsupported type",
                                        ERROR_ID_UNSUPPORTED_TYPE};

const TaskRuntimeError INVALID_BINARY_RECEIVE{
    "Invalid binary received; no more files expected or "
    "current file is complete",
    ERROR_ID_INVALID_BINARY_RECEIVE};

inline TaskRuntimeError LOADING_BINARY_FAILED(const std::string& error)
{
    return {error, ERROR_ID_LOADING_BINARY_FAILED};
}
} // namespace brayns
