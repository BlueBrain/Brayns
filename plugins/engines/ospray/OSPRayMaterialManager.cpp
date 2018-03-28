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

#include "OSPRayMaterialManager.h"

#include <brayns/common/log.h>
#include <brayns/common/material/Material.h>

namespace brayns
{
OSPRayMaterialManager::OSPRayMaterialManager(const uint32_t flags)
    : _memoryManagementFlags(flags)
{
    BRAYNS_FCT_ENTRY
}

void OSPRayMaterialManager::_commitMaterial(OSPMaterial ospMaterial,
                                            Material& material)
{
    if (!material.isModified())
        return;

    BRAYNS_FCT_ENTRY

    auto diffuse = material.getDiffuseColor();
    ospSet3f(ospMaterial, "kd", diffuse.x(), diffuse.y(), diffuse.z());
    auto specular = material.getSpecularColor();
    ospSet3f(ospMaterial, "ks", specular.x(), specular.y(), specular.z());
    ospSet1f(ospMaterial, "ns", material.getSpecularExponent());
    ospSet1f(ospMaterial, "d", material.getOpacity());
    ospSet1f(ospMaterial, "refraction", material.getRefractionIndex());
    ospSet1f(ospMaterial, "reflection", material.getReflectionIndex());
    ospSet1f(ospMaterial, "a", material.getEmission());
    ospSet1f(ospMaterial, "glossiness", material.getGlossiness());
    ospSet1i(ospMaterial, "cast_simulation_data",
             material.getCastSimulationData());
    ospSet1i(ospMaterial, "skybox", material.getType() == MaterialType::skybox);

    for (const auto& textureType : textureTypeMaterialAttribute)
        ospSetObject(ospMaterial, textureType.attribute.c_str(), nullptr);

    // Textures
    for (auto textureType : material.getTextureTypes())
    {
        OSPTexture2D ospTexture = _createTexture2D(textureType.second);
        ospSetObject(
            ospMaterial,
            textureTypeMaterialAttribute[textureType.first].attribute.c_str(),
            ospTexture);
    }
    ospCommit(ospMaterial);
    material.resetModified();
}

OSPTexture2D OSPRayMaterialManager::_createTexture2D(const size_t id)
{
    BRAYNS_FCT_ENTRY
    if (_ospTextures.find(id) != _ospTextures.end())
        return _ospTextures[id];

    Texture2DPtr texture = _textures[id];
    if (!texture)
    {
        BRAYNS_WARN << "Texture " << id << " is not in the cache" << std::endl;
        return nullptr;
    }

    OSPTextureFormat type = OSP_TEXTURE_R8; // smallest valid type as default
    if (texture->getDepth() == 1)
    {
        if (texture->getNbChannels() == 3)
            type = OSP_TEXTURE_SRGB;
        if (texture->getNbChannels() == 4)
            type = OSP_TEXTURE_SRGBA;
    }
    else if (texture->getDepth() == 4)
    {
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
        ospNewTexture2D(texSize, type, texture->getRawData(), 0);

    assert(ospTexture);
    ospCommit(ospTexture);

    _ospTextures[id] = ospTexture;

    return ospTexture;
}

void OSPRayMaterialManager::commit()
{
    BRAYNS_FCT_ENTRY
    if (_ospMaterialData)
        ospRelease(_ospMaterialData);

    for (size_t i = 0; i < _materials.size(); ++i)
    {
        auto& material = _materials[i];
        OSPMaterial ospMaterial = nullptr;
        if (_ospMaterials.size() <= i)
        {
            // OSP Material does not exist, create it
            ospMaterial = ospNewMaterial(nullptr, "ExtendedOBJMaterial");
            _ospMaterials.push_back(ospMaterial);
        }
        else
            ospMaterial = _ospMaterials[i];
        _commitMaterial(ospMaterial, material);
    }
    _ospMaterialData = ospNewData(_ospMaterials.size(), OSP_OBJECT,
                                  &_ospMaterials[0], _memoryManagementFlags);
    ospCommit(_ospMaterialData);
    markModified();
}

OSPMaterial OSPRayMaterialManager::getOSPMaterial(const size_t index)
{
    for (size_t i = _ospMaterials.size() + 1; i < index; ++i)
        _ospMaterials.push_back(ospNewMaterial(nullptr, "ExtendedOBJMaterial"));
    return _ospMaterials[index];
}
}
