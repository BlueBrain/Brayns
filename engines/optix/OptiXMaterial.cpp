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

namespace brayns
{
OptiXMaterial::~OptiXMaterial()
{
    if (_textureSampler)
        _textureSampler->destroy();

    if (_optixMaterial)
        _optixMaterial->destroy();
}

bool OptiXMaterial::isTextured() const
{
    const brayns::TextureDescriptors& descriptors = getTextureDescriptors();
    return descriptors.find(TT_DIFFUSE) != descriptors.end();
}

void OptiXMaterial::commit()
{
    const brayns::TextureDescriptors& descriptors = getTextureDescriptors();
    auto texture_it = descriptors.find(TT_DIFFUSE);
    const bool is_textured = texture_it != descriptors.end();

    if (!_optixMaterial)
        _optixMaterial = OptiXContext::get().createMaterial();

    _optixMaterial["Kd"]->setFloat(_diffuseColor.x(), _diffuseColor.y(),
                                   _diffuseColor.z());
    _optixMaterial["Ks"]->setFloat(_specularColor.x(), _specularColor.y(),
                                   _specularColor.z());
    _optixMaterial["phong_exp"]->setFloat(_specularExponent);
    _optixMaterial["Kr"]->setFloat(_reflectionIndex, _reflectionIndex,
                                   _reflectionIndex);
    _optixMaterial["Ko"]->setFloat(_opacity, _opacity, _opacity);
    _optixMaterial["refraction_index"]->setFloat(_refractionIndex);
    _optixMaterial["glossiness"]->setFloat(_glossiness);

    if (is_textured)
    {
        auto texture = texture_it->second;
        if (!_textureSampler)
            _textureSampler = OptiXContext::get().createTextureSampler(texture);

        _optixMaterial["diffuse_map"]->setTextureSampler(_textureSampler);
    }
}
} // namespace brayns
