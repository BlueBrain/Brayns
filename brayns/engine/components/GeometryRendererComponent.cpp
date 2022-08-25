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

#include "GeometryRendererComponent.h"

namespace brayns
{
Geometry &GeometryRendererComponent::getGeometry() noexcept
{
    return _geometry;
}

void GeometryRendererComponent::setColors(const std::vector<brayns::Vector4f> &colors)
{
    if (colors.size() < _geometry.numPrimitives())
    {
        throw std::invalid_argument("Not enough colors for all geometry");
    }

    _geometryView.setColorPerPrimitive(ospray::cpp::CopiedData(colors));
    _useMaterialColor = false;
}

Bounds GeometryRendererComponent::computeBounds(const Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

void GeometryRendererComponent::onCreate()
{
    Model &model = getModel();
    auto &group = model.getGroup();
    group.fromGeometry(_geometryView);
    model.addComponent<MaterialComponent>();
}

bool GeometryRendererComponent::commit()
{
    auto &model = getModel();
    auto &materialComponent = model.getComponent<MaterialComponent>();
    auto &material = materialComponent.getMaterial();
    if (material.commit())
    {
        _geometryView.setMaterial(material);
    }

    auto commitGeometry = _geometry.commit();
    auto commitView = _geometryView.commit();
    return commitGeometry || commitView;
}
}
