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

#include "DefaultMaterial.h"

#include <brayns/engine/ospray/OsprayMathtypesTraits.h>

namespace
{
class DefaultParameterUpdater
{
public:
    static void update(const brayns::DefaultMaterial &material)
    {
        static const std::string kdParameter = "kd";
        static const std::string nsParameter = "ns";
        static const std::string opacityParameter = "d";

        const auto kd = brayns::Vector3f(1.f);
        constexpr float ns = 2.f;
        auto opacity = material.getOpacity();

        const auto &osprayMaterial = material.getOsprayMaterial();
        osprayMaterial.setParam(kdParameter, kd);
        osprayMaterial.setParam(nsParameter, ns);
        osprayMaterial.setParam(opacityParameter, opacity);
    }
};
}

namespace brayns
{
DefaultMaterial::DefaultMaterial()
    : Material("obj")
{
}

std::string DefaultMaterial::getName() const noexcept
{
    return "default";
}

void DefaultMaterial::setOpacity(const float opacity) noexcept
{
    _updateValue(_opacity, glm::clamp(opacity, 0.f, 1.f));
}

float DefaultMaterial::getOpacity() const noexcept
{
    return _opacity;
}

void DefaultMaterial::commitMaterialSpecificParams()
{
    DefaultParameterUpdater::update(*this);
}
}
