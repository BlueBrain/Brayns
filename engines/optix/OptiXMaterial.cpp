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

#include "OptiXMaterial.h"

#include "CommonStructs.h"
#include "OptiXContext.h"

#include <brayns/common/log.h>

namespace brayns
{
static std::string textureTypeToString[12] = {"albedoMetallic_map",
                                              "normalRoughness_map",
                                              "bump_map",
                                              "aoEmissive_map",
                                              "map_ns",
                                              "map_d",
                                              "map_reflection",
                                              "map_refraction",
                                              "map_occlusion",
                                              "radiance_map",
                                              "irradiance_map",
                                              "brdf_lut"};

OptiXMaterial::~OptiXMaterial()
{
    for (auto& i : _textureSamplers)
        i.second->destroy();

    if (_optixMaterial)
        _optixMaterial->destroy();
}

bool OptiXMaterial::isTextured() const
{
    return !_textureSamplers.empty();
}

void OptiXMaterial::commit()
{
    if (!_optixMaterial)
        _optixMaterial = OptiXContext::get().createMaterial();

    _optixMaterial["Ka"]->setFloat(_emission, _emission, _emission);
    _optixMaterial["Kd"]->setFloat(_diffuseColor.x, _diffuseColor.y,
                                   _diffuseColor.z);
    _optixMaterial["Ks"]->setFloat(_specularColor.x, _specularColor.y,
                                   _specularColor.z);
    _optixMaterial["Kr"]->setFloat(_reflectionIndex, _reflectionIndex,
                                   _reflectionIndex);
    _optixMaterial["Ko"]->setFloat(_opacity, _opacity, _opacity);
    _optixMaterial["glossiness"]->setFloat(_glossiness);
    _optixMaterial["refraction_index"]->setFloat(_refractionIndex);
    _optixMaterial["phong_exp"]->setFloat(_specularExponent);

    for (const auto& i : getTextureDescriptors())
    {
        if (!_textureSamplers.count(i.first))
        {
            auto textureSampler =
                OptiXContext::get().createTextureSampler(i.second);
            _textureSamplers.insert(std::make_pair(i.first, textureSampler));
            _optixMaterial[textureTypeToString[(uint8_t)i.first]]->setInt(
                textureSampler->getId());
        }
    }
}
} // namespace brayns
