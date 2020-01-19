/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <map>

#include <brayns/engineapi/Material.h>

#include <optixu/optixpp_namespace.h>

namespace brayns
{
class OptiXMaterial : public Material
{
public:
    OptiXMaterial() = default;
    ~OptiXMaterial();

    void commit() final;
    bool isTextured() const;

    ::optix::Material getOptixMaterial() { return _optixMaterial; }
    auto getTextureSampler(const TextureType type) const
    {
        return _textureSamplers.at(type);
    }

private:
    ::optix::Material _optixMaterial{nullptr};
    std::map<TextureType, ::optix::TextureSampler> _textureSamplers;
};
} // namespace brayns
