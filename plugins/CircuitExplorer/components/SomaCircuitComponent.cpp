/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "SomaCircuitComponent.h"

#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <api/coloring/ColorByIDAlgorithm.h>

SomaCircuitComponent::SomaCircuitComponent(std::vector<uint64_t> ids, std::vector<brayns::Sphere> geometry)
    : _ids(std::move(ids))
    , _geometry(std::move(geometry))
    , _colors(_ids.size(), brayns::Vector4f(1.f))
{
}

brayns::Bounds SomaCircuitComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

bool SomaCircuitComponent::commit()
{
    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (material.commit())
    {
        _geometry.setMaterial(material);
    }

    return _geometry.commit();
}

void SomaCircuitComponent::onCreate()
{
    auto &model = getModel();
    model.addComponent<brayns::MaterialComponent>();

    auto &group = model.getGroup();
    group.addGeometry(_geometry);
}

void SomaCircuitComponent::onDestroy()
{
    auto &model = getModel();
    auto &group = model.getGroup();
    group.addGeometry(_geometry);
}

void SomaCircuitComponent::onInspect(const brayns::InspectContext &context, brayns::JsonObject &writeResult)
    const noexcept
{
    auto &hittedModel = context.model;
    auto &currentModel = _geometry.getOsprayObject();

    if (hittedModel.handle() != currentModel.handle())
    {
        return;
    }

    auto primitiveIndex = context.primitiveIndex;
    auto cellId = _ids[primitiveIndex];
    writeResult.set("neuron_id", cellId);
}

const std::vector<uint64_t> &SomaCircuitComponent::getIDs() const noexcept
{
    return _ids;
}

void SomaCircuitComponent::setColor(const brayns::Vector4f &color) noexcept
{
    std::fill(_colors.begin(), _colors.end(), color);
    _geometry.setColorPerPrimitive(ospray::cpp::SharedData(_colors));
}

void SomaCircuitComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    if (colors.size() < _geometry.getNumPrimitives())
    {
        throw std::invalid_argument("Not enough colors for all geometry");
    }

    _colors = colors;
    _geometry.setColorPerPrimitive(ospray::cpp::SharedData(_colors));
}

std::vector<uint64_t> SomaCircuitComponent::setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept
{
    auto skipped = ColorByIDAlgorithm::execute(
        colors,
        _ids,
        [&](uint64_t id, size_t index, const brayns::Vector4f &color)
        {
            (void)id;
            _colors[index] = color;
        });

    if (skipped.size() < _ids.size())
    {
        _geometry.setColorPerPrimitive(ospray::cpp::SharedData(_colors));
    }

    return skipped;
}

void SomaCircuitComponent::setIndexedColor(
    const std::vector<brayns::Vector4f> &color,
    const std::vector<uint8_t> &mapping)
{
    if (color.size() > 256)
    {
        throw std::invalid_argument("Color map has more than 256 values");
    }
    _geometry.setColorMap(ospray::cpp::CopiedData(color), ospray::cpp::CopiedData(mapping));
}
