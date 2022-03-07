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

#pragma once

#include <brayns/engine/defaultcomponents/GeometryRendererComponent.h>
#include <brayns/engine/geometries/Sphere.h>

#include <ospray/ospray.h>

namespace brayns
{
class ProteinRendererComponent final : public GeometryRendererComponent<Sphere>
{
public:
    ProteinRendererComponent(const std::vector<Sphere>& spheres,
                             std::vector<Vector4f>&& colorMap,
                             std::vector<uint8_t>&& colorMapIndices);

    uint64_t getSizeInBytes() const noexcept override;

    void onStart() override;

private:
    std::vector<Vector4f> _colorMap;
    std::vector<uint8_t> _colorMapIndices;
};
}
