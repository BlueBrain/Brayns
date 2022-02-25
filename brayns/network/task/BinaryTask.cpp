/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include "BinaryTask.h"

#include <brayns/common/Log.h>

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace
{
static const brayns::RequestId emptyId;
static const std::string binaryMethod = "<binary request>";
} // namespace

namespace brayns
{
BinaryTask::BinaryTask(ClientRequest request, BinaryManager &binary)
    : _request(std::move(request))
    , _binary(binary)
{
}

const ClientRef &BinaryTask::getClient() const
{
    return _request.getClient();
}

const RequestId &BinaryTask::getId() const
{
    return emptyId;
}

const std::string &BinaryTask::getMethod() const
{
    return binaryMethod;
}

void BinaryTask::run()
{
    Log::info("Buffering binary request {}.", _request);
    _binary.add(std::move(_request));
}

void BinaryTask::cancel()
{
    Log::error("Trying to cancel a binary request.");
    throw TaskNotCancellableException(binaryMethod);
}
} // namespace brayns
