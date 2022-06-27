/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "ImageOperationManager.h"
#include "operations/ToneMapping.h"

#include <algorithm>

namespace brayns
{
ImageOperationManager::ImageOperationManager()
{
    // Currently hardcoded, since is the only opration and we want it always enabled
    addOperation(std::make_unique<ToneMapping>());
}

uint32_t ImageOperationManager::addOperation(std::unique_ptr<ImageOperation> operation) noexcept
{
    _operations.emplace_back();
    auto &entry = _operations.back();

    entry.id = _idFactory.generateID();
    entry.operation = std::move(operation);

    entry.operation->commit();

    return entry.id;
}

void ImageOperationManager::removeOperation(const uint32_t id)
{
    auto begin = _operations.begin();
    auto end = _operations.end();
    auto it = std::find_if(begin, end, [opId = id](OperationEntry &entry) { return opId == entry.id; });

    if (it == end)
    {
        throw std::invalid_argument("No image operation exists with id " + std::to_string(id));
    }

    _operations.erase(it);
}

std::vector<ospray::cpp::ImageOperation> ImageOperationManager::getOperationHandles() const noexcept
{
    std::vector<ospray::cpp::ImageOperation> handles;
    handles.reserve(_operations.size());

    for (const auto &entry : _operations)
    {
        auto &operation = *(entry.operation);
        const auto &object = operation.getOsprayObject();
        handles.push_back(object);
    }
    return handles;
}
}
