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

#include <vector>

#include <ospray/ospray_cpp.h>

#include <brayns/core/utils/Math.h>

namespace brayns
{
class TransferFunction
{
public:
    explicit TransferFunction(ospray::cpp::TransferFunction function);

    ospray::cpp::TransferFunction getHandle() const;
    void setColors(const std::vector<Color3> &colors);
    void setOpacities(const std::vector<float> &opacities);
    void setScalarRange(Box1 range);
    void commit();

private:
    ospray::cpp::TransferFunction _function;
};

class VolumeModel
{
public:
    explicit VolumeModel(ospray::cpp::VolumetricModel model);

    ospray::cpp::VolumetricModel getHandle() const;
    void setId(std::uint32_t id);
    void setTransferFunction(ospray::cpp::TransferFunction function);
    void commit();

private:
    ospray::cpp::VolumetricModel _model;
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

class StructuredRegularVolume
{
public:
    explicit StructuredRegularVolume(ospray::cpp::Volume volume);

    ospray::cpp::Volume getHandle() const;
    void setData(const VolumeData &data);
    void setType(VolumeType type);
    void setOrigin(const Vector3 &origin);
    void setSpacing(const Vector3 &spacing);
    void setFilter(VolumeFilter filter);
    void commit();

private:
    ospray::cpp::Volume _volume;
};
}
