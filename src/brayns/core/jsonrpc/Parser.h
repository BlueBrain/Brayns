/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <span>
#include <string>
#include <vector>

#include "Errors.h"
#include "Messages.h"

namespace brayns
{
JsonRpcRequest parseJsonRpcRequest(const std::string &text);
JsonRpcRequest parseJsonRpcRequest(const std::vector<char> &binary);
std::string composeAsText(const JsonRpcSuccessResponse &response);
std::string composeAsText(const JsonRpcErrorResponse &response);
std::vector<char> composeAsBinary(const JsonRpcSuccessResponse &response);
JsonRpcError composeError(const JsonRpcException &e);
JsonRpcErrorResponse composeErrorResponse(const JsonRpcException &e, std::optional<JsonRpcId> id = {});
}
