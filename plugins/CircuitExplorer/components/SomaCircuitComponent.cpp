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

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <api/coloring/ColorByIDAlgorithm.h>

brayns::Bounds SomaCircuitComponent::computeBounds(const brayns::Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

bool SomaCircuitComponent::commit()
{
    bool needsCommit = false;

    auto &material = brayns::ExtractModelObject::extractMaterial(getModel());
    if (material.commit())
    {
        brayns::GeometricModelHandler::setMaterial(_model, material);
        needsCommit = true;
    }

    needsCommit = needsCommit || _colorsDirty;
    _colorsDirty = false;

    if (needsCommit)
    {
        brayns::GeometricModelHandler::commitModel(_model);
    }

    return needsCommit;
}

void SomaCircuitComponent::onDestroy()
{
    brayns::GeometricModelHandler::removeFromGeometryGroup(_model, getModel());
    brayns::GeometricModelHandler::destroy(_model);
}

void SomaCircuitComponent::setSomas(std::vector<uint64_t> ids, std::vector<brayns::Sphere> geometry) noexcept
{
    _ids = std::move(ids);

    _model = brayns::GeometricModelHandler::create();

    auto &group = getModel();
    brayns::GeometricModelHandler::addToGeometryGroup(_model, group);

    auto &materialComponent = group.addComponent<brayns::MaterialComponent>();
    auto &material = materialComponent.getMaterial();
    material.commit();
    brayns::GeometricModelHandler::setMaterial(_model, material);

    _geometry.set(std::move(geometry));
    _geometry.commit();
    brayns::GeometricModelHandler::setGeometry(_model, _geometry);

    _colors.resize(_ids.size());
    setColor(brayns::Vector4f(1.f));
}

const std::vector<uint64_t> &SomaCircuitComponent::getIDs() const noexcept
{
    return _ids;
}

void SomaCircuitComponent::setColor(const brayns::Vector4f &color) noexcept
{
    std::fill(_colors.begin(), _colors.end(), color);
    auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    _colorsDirty = true;
}

void SomaCircuitComponent::setColorById(const std::vector<brayns::Vector4f> &colors)
{
    if (colors.size() < _geometry.getNumGeometries())
    {
        throw std::invalid_argument("Not enough colors for all geometry");
    }

    _colors = colors;

    auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    _colorsDirty = true;
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
            _colorsDirty = true;
        });

    if (_colorsDirty)
    {
        auto colorBuffer = brayns::DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
        brayns::GeometricModelHandler::setColors(_model, colorBuffer);
    }

    return skipped;
}

void SomaCircuitComponent::setIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &mapping)
{
    if (color.size > 256)
    {
        throw std::invalid_argument("Color map has more than 256 values");
    }
    auto indexData = brayns::DataHandler::shareBuffer(mapping, OSPDataType::OSP_UCHAR);
    brayns::GeometricModelHandler::setColorMap(_model, color, indexData);
    _colorsDirty = true;
}
