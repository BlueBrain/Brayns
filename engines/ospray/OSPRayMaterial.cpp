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

#include "OSPRayMaterial.h"

#include <brayns/common/log.h>

#include <cassert>

namespace brayns
{
struct TextureTypeMaterialAttribute
{
    TextureType type;
    std::string attribute;
};

static TextureTypeMaterialAttribute textureTypeMaterialAttribute[8] = {
    {TT_DIFFUSE, "map_kd"},
    {TT_NORMALS, "map_bump"},
    {TT_BUMP, "map_bump"},
    {TT_SPECULAR, "map_ks"},
    {TT_EMISSIVE, "map_ns"},
    {TT_OPACITY, "map_d"},
    {TT_REFLECTION, "map_reflection"},
    {TT_REFRACTION, "map_refraction"}};

OSPRayMaterial::OSPRayMaterial()
    : _ospMaterial(ospNewMaterial(nullptr, "ExtendedOBJMaterial"))
{
}

OSPRayMaterial::~OSPRayMaterial()
{
    if (_ospMaterial)
        ospRelease(_ospMaterial);
}

void OSPRayMaterial::commit()
{
    if (!isModified())
        return;

    ospSet3f(_ospMaterial, "kd", _diffuseColor.x(), _diffuseColor.y(),
             _diffuseColor.z());
    ospSet3f(_ospMaterial, "ks", _specularColor.x(), _specularColor.y(),
             _specularColor.z());
    ospSet1f(_ospMaterial, "ns", _specularExponent);
    ospSet1f(_ospMaterial, "d", _opacity);
    ospSet1f(_ospMaterial, "refraction", _refractionIndex);
    ospSet1f(_ospMaterial, "reflection", _reflectionIndex);
    ospSet1f(_ospMaterial, "a", _emission);
    ospSet1f(_ospMaterial, "glossiness", _glossiness);
    ospSet1i(_ospMaterial, "cast_simulation_data", _castSimulationData);
    ospSet1i(_ospMaterial, "shading_mode", static_cast<uint16_t>(_shadingMode));

    // Textures
    for (const auto& textureType : textureTypeMaterialAttribute)
        ospSetObject(_ospMaterial, textureType.attribute.c_str(), nullptr);

    for (const auto& textureDescriptor : _textureDescriptors)
    {
        const auto texType = textureDescriptor.first;
        auto texture = getTexture(texType);
        if (texture)
        {
            auto ospTexture = _createOSPTexture2D(texture);
            const auto str =
                textureTypeMaterialAttribute[texType].attribute.c_str();
            ospSetObject(_ospMaterial, str, ospTexture);
            ospRelease(ospTexture);
        }
    }

    ospCommit(_ospMaterial);
    resetModified();
}

OSPTexture2D OSPRayMaterial::_createOSPTexture2D(Texture2DPtr texture)
{
    OSPTextureFormat type = OSP_TEXTURE_R8; // smallest valid type as default
    if (texture->getDepth() == 1)
    {
        if (texture->getNbChannels() == 1)
            type = OSP_TEXTURE_R8;
        if (texture->getNbChannels() == 3)
            type = OSP_TEXTURE_RGB8;
        if (texture->getNbChannels() == 4)
            type = OSP_TEXTURE_RGBA8;
    }
    else if (texture->getDepth() == 4)
    {
        if (texture->getNbChannels() == 1)
            type = OSP_TEXTURE_R32F;
        if (texture->getNbChannels() == 3)
            type = OSP_TEXTURE_RGB32F;
        if (texture->getNbChannels() == 4)
            type = OSP_TEXTURE_RGBA32F;
    }

    BRAYNS_DEBUG << "Creating OSPRay texture from " << texture->getFilename()
                 << ": " << texture->getWidth() << "x" << texture->getHeight()
                 << "x" << (int)type << std::endl;

    osp::vec2i texSize{int(texture->getWidth()), int(texture->getHeight())};
    OSPTexture2D ospTexture =
        ospNewTexture2D(texSize, type, texture->getRawData(),
                        OSP_TEXTURE_SHARED_BUFFER);

    assert(ospTexture);
    ospCommit(ospTexture);

    return ospTexture;
}
} // namespace brayns
