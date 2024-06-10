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

#include "Data.h"
#include "Object.h"

namespace brayns::experimental
{
class TransferFunction : public Managed<OSPTransferFunction>
{
public:
    using Managed::Managed;
};

struct LinearTransferFunctionSettings
{
    std::span<Color3> colors;
    std::span<float> opacities;
    Box1 scalarRange;
};

class LinearTransferFunction : public TransferFunction
{
public:
    using TransferFunction::TransferFunction;
};

template<>
struct ObjectReflector<LinearTransferFunction>
{
    using Settings = LinearTransferFunctionSettings;

    static OSPTransferFunction createHandle(OSPDevice device, const Settings &settings);
};
}
