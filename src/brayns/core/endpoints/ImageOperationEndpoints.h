/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include <any>
#include <functional>

#include <brayns/core/api/ApiBuilder.h>
#include <brayns/core/engine/ImageOperation.h>
#include <brayns/core/manager/LockedObjects.h>

namespace brayns
{
struct ImageOperationInterface
{
    std::any value;
    std::function<std::string()> getType;
    std::function<ImageOperation()> getDeviceObject;
};

template<>
struct ObjectReflector<ImageOperationInterface>
{
    static std::string getType(const ImageOperationInterface &operation)
    {
        return operation.getType();
    }
};

void addImageOperationEndpoints(ApiBuilder &builder, LockedObjects &objects, Device &device);
}