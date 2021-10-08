/* Copyright (c) 2020, EPFL/Blue Brain Project
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

#include "PBRTConstants.h"

#include <brayns/engine/Material.h>

#include <pbrt/core/material.h>

#include <functional>
#include <unordered_map>

namespace brayns
{
class PBRTMaterial : public Material
{
public:
    PBRTMaterial(const PropertyMap& properties = {});
    ~PBRTMaterial() = default;

    void commit(const std::string& renderer);

    void commit() final;

    std::shared_ptr<pbrt::Material> getPBRTMaterial() { return _pbrtMat; }

private:
    bool _checkMaterialIntegrator(const PBRTMaterialClass matClass) const;

    template <typename MaterialClass>
    pbrt::Material* _createPBRTMaterial() const;
    pbrt::Material* _instantiateMaterial(
        const PBRTMaterialClass matClass) const;

    std::shared_ptr<pbrt::Material> _pbrtMat{nullptr};
    std::string _renderer;
};
} // namespace brayns
