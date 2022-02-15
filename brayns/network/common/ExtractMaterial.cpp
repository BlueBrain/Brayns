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

#include "ExtractMaterial.h"

#include <brayns/network/jsonrpc/JsonRpcException.h>

namespace brayns
{
Material &ExtractMaterial::fromId(ModelDescriptor &descriptor, size_t id)
{
    auto &model = descriptor.getModel();
    auto modelId = descriptor.getModelID();
    return fromId(model, modelId, id);
}

Material &ExtractMaterial::fromId(Model &model, size_t modelId, size_t id)
{
    auto material = model.getMaterial(id);
    if (!material)
    {
        throw JsonRpcException("No material with ID " + std::to_string(id) + " in model " + std::to_string(modelId));
    }
    return *material;
}
} // namespace brayns
