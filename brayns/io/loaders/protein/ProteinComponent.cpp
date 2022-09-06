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

#include <brayns/engine/common/ExtractComponent.h>
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>
#include <brayns/engine/model/Model.h>

#include <ospray/ospray_cpp/Data.h>

namespace brayns
{
ProteinComponent::ProteinComponent(
    std::vector<Sphere> spheres,
    std::vector<uint8_t> indices,
    std::vector<Vector4f> colors)
    : _geometry(std::move(spheres))
    , _geometryView(_geometry)
    , _indices(std::move(indices))
    , _colors(std::move(colors))
{
    _geometryView.setColorMap(ospray::cpp::SharedData(_indices), ospray::cpp::SharedData(_colors));
}

Bounds ProteinComponent::computeBounds(const Matrix4f &transform) const noexcept
{
    return _geometry.computeBounds(transform);
}

void ProteinComponent::onCreate()
{
    auto &model = getModel();
    auto &group = model.getGroup();
    group.setGeometry(_geometryView);
    model.addComponent<MaterialComponent>();
}

bool ProteinComponent::commit()
{
    auto &material = ExtractComponent::material(getModel());
    if (material.commit())
    {
        _geometryView.setMaterial(material);
    }

    auto commitGeometry = _geometry.commit();
    auto commitView = _geometryView.commit();
    return commitGeometry || commitView;
}
}
