/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "Material.h"

#include <string>

namespace
{
using namespace brayns::experimental;

void setTextureParam(OSPMaterial handle, const char *id, std::monostate)
{
    removeObjectParam(handle, id);
}

void setTextureParam(OSPMaterial handle, const char *id, const MaterialTexture2D &texture)
{
    setObjectParam(handle, id, texture.value);

    auto name = std::string(id);

    auto rotation = name + ".rotation";
    auto scale = name + ".scale";
    auto translation = name + ".translation";

    setObjectParam(handle, rotation.c_str(), texture.transform.rotation);
    setObjectParam(handle, scale.c_str(), texture.transform.scale);
    setObjectParam(handle, translation.c_str(), texture.transform.translation);
}

void setTextureParam(OSPMaterial handle, const char *id, const MaterialVolumeTexture &texture)
{
    setObjectParam(handle, id, texture.value);

    auto name = std::string(id);

    auto transform = name + ".transform";

    setObjectParam(handle, transform.c_str(), toAffine(texture.transform));
}

void setTextureParam(OSPMaterial handle, const char *id, const MaterialTexture &texture)
{
    std::visit([=](const auto &value) { setTextureParam(handle, id, value); }, texture);
}

void setMaterialParams(OSPMaterial handle, const AmbientOcclusionMaterialSettings &settings)
{
    setObjectParam(handle, "kd", settings.diffuse);
    setTextureParam(handle, "map_kd", settings.diffuseMap);
    setObjectParam(handle, "d", settings.opacity);
}
}

namespace brayns::experimental
{
void loadMaterialParams(OSPMaterial handle, const AmbientOcclusionMaterialSettings &settings)
{
    setMaterialParams(handle, settings);
    commitObject(handle);
}

void loadMaterialParams(OSPMaterial handle, const ScivisMaterialSettings &settings)
{
    setMaterialParams(handle, settings);
    setObjectParam(handle, "ks", settings.specular);
    setObjectParam(handle, "ns", settings.shininess);
    setObjectParam(handle, "tf", settings.transparencyFilter);
    commitObject(handle);
}

void loadMaterialParams(OSPMaterial handle, const PrincipledMaterialSettings &settings)
{
    setObjectParam(handle, "baseColor", settings.baseColor);
    setTextureParam(handle, "map_baseColor", settings.baseColorMap);
    setObjectParam(handle, "edgeColor", settings.edgeColor);
    setObjectParam(handle, "metallic", settings.metallic);
    setObjectParam(handle, "diffuse", settings.diffuse);
    setObjectParam(handle, "specular", settings.specular);
    setObjectParam(handle, "ior", settings.ior);
    setObjectParam(handle, "transmission", settings.transmission);
    setObjectParam(handle, "transmissionColor", settings.transmissionColor);
    setObjectParam(handle, "transmissionDepth", settings.transmissionDepth);
    setObjectParam(handle, "roughness", settings.roughness);
    setObjectParam(handle, "anisotropy", settings.anisotropy);
    setObjectParam(handle, "rotation", settings.rotation);
    setObjectParam(handle, "normal", settings.normal);
    setTextureParam(handle, "map_normal", settings.normalMap);
    setObjectParam(handle, "baseNormal", settings.baseNormal);
    setObjectParam(handle, "thin", settings.thin);
    setObjectParam(handle, "thickness", settings.thickness);
    setObjectParam(handle, "backlight", settings.backlight);
    setObjectParam(handle, "coat", settings.coat);
    setObjectParam(handle, "coatIor", settings.coatIor);
    setObjectParam(handle, "coatColor", settings.coatColor);
    setObjectParam(handle, "coatThickness", settings.coatThickness);
    setObjectParam(handle, "coatRoughness", settings.coatRoughness);
    setObjectParam(handle, "coatNormal", settings.coatNormal);
    setObjectParam(handle, "sheen", settings.sheen);
    setObjectParam(handle, "sheenColor", settings.sheenColor);
    setObjectParam(handle, "sheenTint", settings.sheenTint);
    setObjectParam(handle, "sheenRoughness", settings.sheenRoughness);
    setObjectParam(handle, "opacity", settings.opacity);
    setObjectParam(handle, "emissiveColor", settings.emissiveColor);
    commitObject(handle);
}
}
