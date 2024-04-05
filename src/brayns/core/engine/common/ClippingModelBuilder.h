/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/core/engine/components/ClipperViews.h>
#include <brayns/core/engine/components/Geometries.h>
#include <brayns/core/engine/model/Model.h>
#include <brayns/core/engine/systems/ClipperDataSystem.h>

namespace brayns
{
class ClippingModelBuilder
{
public:
    template<typename T>
    static std::shared_ptr<Model> build(std::vector<T> primitives, bool invertNormals)
    {
        auto model = std::make_shared<brayns::Model>("clip_geometry");

        auto &components = model->getComponents();
        auto &geometries = components.add<brayns::Geometries>(std::move(primitives));
        auto &geometry = geometries.elements.back();

        auto &views = components.add<brayns::ClipperViews>(geometry);
        auto &view = views.elements.back();
        view.setNormalsInverted(invertNormals);

        auto &systems = model->getSystems();
        systems.setDataSystem<brayns::ClipperDataSystem>();

        return model;
    }
};
}
