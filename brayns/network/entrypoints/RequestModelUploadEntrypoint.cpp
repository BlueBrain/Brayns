/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include "RequestModelUploadEntrypoint.h"

#include <brayns/network/binary/ModelUploadTask.h>

namespace brayns
{
std::string RequestModelUploadEntrypoint::getName() const
{
    return "request-model-upload";
}

std::string RequestModelUploadEntrypoint::getDescription() const
{
    return "Request model upload from further received blobs and return "
           "model descriptor on success";
}

bool RequestModelUploadEntrypoint::isAsync() const
{
    return true;
}

void RequestModelUploadEntrypoint::onRequest(const Request &request)
{
    auto &engine = getApi().getEngine();
    auto &registry = getApi().getLoaderRegistry();
    auto task = std::make_shared<ModelUploadTask>(engine, registry);
    launchTask(task, request);
    auto &binary = getBinary();
    auto &handle = request.getConnectionHandle();
    binary.addTask(handle, task);
}
} // namespace brayns
