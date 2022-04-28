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

#include "ProteinComponent.h"

#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/common/SizeHelper.h>
#include <brayns/engine/components/MaterialComponent.h>

namespace brayns
{
ProteinComponent::ProteinComponent(std::vector<Sphere> sphe, std::vector<Vector4f> colors, std::vector<uint8_t> indx)
    : _colors(std::move(colors))
    , _colorIndices(std::move(indx))
{
    _geometry.set(std::move(sphe));
}

Bounds ProteinComponent::computeBounds(const Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

void ProteinComponent::onCreate()
{
    _model = GeometricModelHandler::create();

    auto &group = getModel();
    GeometricModelHandler::addToGeometryGroup(_model, group);

    group.addComponent<MaterialComponent>();

    GeometricModelHandler::setGeometry(_model, _geometry);

    auto colorData = DataHandler::shareBuffer(_colors, OSPDataType::OSP_VEC4F);
    auto indexData = DataHandler::shareBuffer(_colorIndices, OSPDataType::OSP_UCHAR);
    GeometricModelHandler::setColorMap(_model, colorData, indexData);
}

bool ProteinComponent::commit()
{
    bool needsCommit = false;

    auto &material = ExtractModelObject::extractMaterial(getModel());
    if (material.commit())
    {
        GeometricModelHandler::setMaterial(_model, material);
        needsCommit = true;
    }

    if (_geometry.commit())
    {
        needsCommit = true;
    }

    if (needsCommit)
    {
        GeometricModelHandler::commitModel(_model);
    }

    return needsCommit;
}

void ProteinComponent::onDestroy()
{
    GeometricModelHandler::removeFromGeometryGroup(_model, getModel());
    GeometricModelHandler::destroy(_model);
}
}
