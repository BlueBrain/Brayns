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
#include "utils.h"

#include <brayns/common/log.h>

#include <ospray/SDK/common/OSPCommon.h>

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

OSPRayMaterial::~OSPRayMaterial()
{
    ospRelease(_ospMaterial);
}

void OSPRayMaterial::commit()
{
    // Do nothing until this material is instanced for a specific renderer
    if (!_ospMaterial || !isModified())
        return;

    if (getCurrentType() == "simulation")
        osphelper::set(_ospMaterial, "apply_simulation", true);
    else
        ospRemoveParam(_ospMaterial, "apply_simulation");

    osphelper::set(_ospMaterial, "kd", Vector3f(_diffuseColor));
    osphelper::set(_ospMaterial, "ks", Vector3f(_specularColor));
    osphelper::set(_ospMaterial, "ns", static_cast<float>(_specularExponent));
    osphelper::set(_ospMaterial, "d", static_cast<float>(_opacity));
    osphelper::set(_ospMaterial, "refraction",
                   static_cast<float>(_refractionIndex));
    osphelper::set(_ospMaterial, "reflection",
                   static_cast<float>(_reflectionIndex));
    osphelper::set(_ospMaterial, "a", static_cast<float>(_emission));
    osphelper::set(_ospMaterial, "glossiness", static_cast<float>(_glossiness));
    osphelper::set(_ospMaterial, "skybox", _isBackGroundMaterial);

    // Properties
    toOSPRayProperties(*this, _ospMaterial);

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

void OSPRayMaterial::commit(const std::string& renderer)
{
    if (!isModified())
        return;
    ospRelease(_ospMaterial);
    _ospMaterial = ospNewMaterial2(renderer.c_str(), "default_material");
    markModified(false); // Ensure commit recreates the ISPC object
    commit();
}

OSPTexture OSPRayMaterial::_createOSPTexture2D(Texture2DPtr texture)
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

    OSPTexture ospTexture = ospNewTexture("texture2d");

    const Vector2i size{int(texture->getWidth()), int(texture->getHeight())};

    osphelper::set(ospTexture, "type", static_cast<int>(type));
    osphelper::set(ospTexture, "size", size);
    auto textureData =
        ospNewData(texture->getSizeInBytes(), OSP_RAW, texture->getRawData(),
                   OSP_DATA_SHARED_BUFFER);
    ospSetObject(ospTexture, "data", textureData);
    ospRelease(textureData);
    ospCommit(ospTexture);

    return ospTexture;
}
} // namespace brayns
