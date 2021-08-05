/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include <brayns/network/entrypoint/ExtractModel.h>
#include <brayns/network/json/JsonBuffer.h>

#include "TransferFunctionAdapter.h"

namespace brayns
{
class ModelTransferFunction
{
public:
    using Buffer = JsonBuffer<TransferFunction>;

    ModelTransferFunction() = default;

    ModelTransferFunction(Scene& scene)
        : _scene(&scene)
    {
    }

    void setId(size_t id) { ExtractModel::fromId(*_scene, id); }

    void setTransferFunction(const Buffer& buffer)
    {
        auto& transferFunction = _scene->getTransferFunction();
        buffer.deserialize(transferFunction);
    }

private:
    Scene* _scene = nullptr;
};

BRAYNS_ADAPTER_BEGIN(ModelTransferFunction)
BRAYNS_ADAPTER_SET("id", setId, "Model ID", Required())
BRAYNS_ADAPTER_SET("transfer_function", setTransferFunction,
                   "Transfer function", Required())
BRAYNS_ADAPTER_END()
} // namespace brayns