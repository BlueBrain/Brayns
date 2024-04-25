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

#include "Object.h"

namespace brayns
{
class BaseTransferFunction : public Object<ospray::cpp::TransferFunction>
{
public:
    using Object::Object;
};

class LinearTransferFunction : public BaseTransferFunction
{
public:
    using BaseTransferFunction::BaseTransferFunction;

    static inline const std::string name = "piecewiseLinear";

    void setColors(SharedArray<Color3> colors);
    void setOpacities(SharedArray<float> opacities);
    void setScalarRange(Box1 range);
};

class BaseVolume : public Object<ospray::cpp::Volume>
{
public:
    using Object::getBounds;
    using Object::Object;
};

class VolumetricModel : public Object<ospray::cpp::VolumetricModel>
{
public:
    using Object::getBounds;
    using Object::Object;

    void setVolume(const BaseVolume &volume);
    void setTransferFunction(const BaseTransferFunction &function);
    void setId(std::uint32_t id);
};

enum class VoxelDataType
{
    UChar = OSP_UCHAR,
    Short = OSP_SHORT,
    UShort = OSP_USHORT,
    Half = OSP_HALF,
    Float = OSP_FLOAT,
    Double = OSP_DOUBLE,
};

struct VolumeData
{
    const void *data;
    VoxelDataType type;
    Size3 size;
};

enum class VolumeType
{
    VertexCentered,
    CellCentered,
};

enum class VolumeFilter
{
    Nearest = OSP_VOLUME_FILTER_NEAREST,
    Linear = OSP_VOLUME_FILTER_LINEAR,
    Cubic = OSP_VOLUME_FILTER_CUBIC,
};

class StructuredRegularVolume : public BaseVolume
{
public:
    using BaseVolume::BaseVolume;

    static inline const std::string name = "structuredRegular";

    void setData(const VolumeData &data);
    void setType(VolumeType type);
    void setOrigin(const Vector3 &origin);
    void setSpacing(const Vector3 &spacing);
    void setFilter(VolumeFilter filter);
};
}
