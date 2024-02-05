/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/TextureType.h>

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
    {TextureType::diffuse, "map_kd"},
    {TextureType::normals, "map_bump"},
    {TextureType::bump, "map_bump"},
    {TextureType::specular, "map_ks"},
    {TextureType::emissive, "map_ns"},
    {TextureType::opacity, "map_d"},
    {TextureType::reflection, "map_reflection"},
    {TextureType::refraction, "map_refraction"}};

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

    ospCommit(_ospMaterial);
    resetModified();
}

void OSPRayMaterial::commit(const std::string& renderer)
{
    if (!isModified() && renderer == _renderer)
        return;
    ospRelease(_ospMaterial);
    _ospMaterial = ospNewMaterial2(renderer.c_str(), "default");
    if (!_ospMaterial)
        throw std::runtime_error("Could not create material for renderer '" +
                                 renderer + "'");
    _renderer = renderer;
    markModified(false); // Ensure commit recreates the ISPC object
    commit();
}
} // namespace brayns
