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

#pragma once

#include "RendererTraits.h"

#include <ospray/ospray_cpp/Renderer.h>

#include <memory>

namespace brayns
{
class IRendererData
{
public:
    virtual ~IRendererData() = default;
    virtual void pushTo(ospray::cpp::Renderer &handle) = 0;
    virtual int32_t getSamplesPerPixel() const noexcept = 0;
    virtual std::unique_ptr<IRendererData> clone() const noexcept = 0;
};

template<typename DataType>
class RendererData final : public IRendererData
{
public:
    DataWrapper(DataType value)
        : data(std::move(value))
    {
    }

    void pushTo(ospray::cpp::Renderer &handle) override
    {
        RendererTraits<DataType>::updateData(handle, data);
    }

    int32_t getSamplesPerPixel() const noexcept override
    {
        return data.samplesPerPixel;
    }

    std::unique_ptr<IRendererData> clone() const noexcept override
    {
        return std::make_unique<RendererData<DataType>>(data);
    }

    DataType data;
};
}
