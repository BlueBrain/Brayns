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

#pragma once

#include <brayns/engine/scenecomponents/SceneModelManager.h>

#include <brayns/json/JsonBuffer.h>

#include "TransferFunctionAdapter.h"

namespace brayns
{
class ModelTransferFunction
{
public:
    ModelTransferFunction() = default;
    ModelTransferFunction(SceneModelManager &smm);

    void setId(const uint32_t id);
    void setTransferFunction(const JsonBuffer<TransferFunction> &buffer);

private:
    SceneModelManager *_smm = nullptr;
    uint32_t _modelId {std::numeric_limits<uint32_t>::max()};
    TransferFunction* _transferFunction;
};

BRAYNS_JSON_ADAPTER_BEGIN(ModelTransferFunction)
BRAYNS_JSON_ADAPTER_SET("id", setId, "Model ID", Required())
BRAYNS_JSON_ADAPTER_SET("transfer_function", setTransferFunction, "Transfer function", Required())
BRAYNS_JSON_ADAPTER_END()
} // namespace brayns
