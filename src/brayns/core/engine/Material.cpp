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
using namespace brayns;

void setTextureParam(OSPMaterial handle, const std::string &id, std::monostate)
{
    removeObjectParam(handle, id.c_str());
}

void setTextureParam(OSPMaterial handle, const std::string &id, const MaterialTexture2D &texture)
{
    setObjectParam(handle, id.c_str(), texture.value);

    auto rotation = id + ".rotation";
    setObjectParam(handle, rotation.c_str(), texture.transform.rotation);

    auto scale = id + ".scale";
    setObjectParam(handle, scale.c_str(), texture.transform.scale);

    auto translation = id + ".translation";
    setObjectParam(handle, translation.c_str(), texture.transform.translation);
}

void setTextureParam(OSPMaterial handle, const std::string &id, const MaterialVolumeTexture &texture)
{
    setObjectParam(handle, id.c_str(), texture.value);

    auto transform = id + ".transform";
    setObjectParam(handle, transform.c_str(), toAffine(texture.transform));
}

void setTextureParam(OSPMaterial handle, const std::string &id, const MaterialTexture &texture)
{
    std::visit([&](const auto &value) { setTextureParam(handle, id, value); }, texture);
}

template<typename T>
void setMaterialParam(OSPMaterial handle, const char *id, const MaterialField<T> &param)
{
    setObjectParam(handle, id, param.factor);

    auto textureId = std::string("map_") + id;
    setTextureParam(handle, textureId, param.texture);
}
}

namespace brayns
{
AoMaterial createAoMaterial(Device &device, const AoMaterialSettings &settings)
{
    auto handle = ospNewMaterial("obj");
    auto material = wrapObjectHandleAs<AoMaterial>(device, handle);

    setMaterialParam(handle, "kd", settings.diffuse);
    setMaterialParam(handle, "d", settings.opacity);

    commitObject(handle);

    return material;
}

ScivisMaterial createScivisMaterial(Device &device, const ScivisMaterialSettings &settings)
{
    auto handle = ospNewMaterial("obj");
    auto material = wrapObjectHandleAs<ScivisMaterial>(device, handle);

    setMaterialParam(handle, "kd", settings.diffuse);
    setMaterialParam(handle, "d", settings.opacity);
    setMaterialParam(handle, "ks", settings.specular);
    setMaterialParam(handle, "ns", settings.shininess);
    setObjectParam(handle, "tf", settings.transparencyFilter);

    commitObject(handle);

    return material;
}

PrincipledMaterial createPrincipledMaterial(Device &device, const PrincipledMaterialSettings &settings)
{
    auto handle = ospNewMaterial("principled");
    auto material = wrapObjectHandleAs<PrincipledMaterial>(device, handle);

    setMaterialParam(handle, "baseColor", settings.baseColor);
    setMaterialParam(handle, "edgeColor", settings.edgeColor);
    setMaterialParam(handle, "metallic", settings.metallic);
    setMaterialParam(handle, "diffuse", settings.diffuse);
    setMaterialParam(handle, "specular", settings.specular);
    setMaterialParam(handle, "ior", settings.ior);
    setMaterialParam(handle, "transmission", settings.transmission);
    setMaterialParam(handle, "transmissionColor", settings.transmissionColor);
    setMaterialParam(handle, "transmissionDepth", settings.transmissionDepth);
    setMaterialParam(handle, "roughness", settings.roughness);
    setMaterialParam(handle, "anisotropy", settings.anisotropy);
    setMaterialParam(handle, "rotation", settings.rotation);
    setMaterialParam(handle, "normal", settings.normal);
    setMaterialParam(handle, "baseNormal", settings.baseNormal);
    setMaterialParam(handle, "thin", settings.thin);
    setMaterialParam(handle, "thickness", settings.thickness);
    setMaterialParam(handle, "backlight", settings.backlight);
    setMaterialParam(handle, "coat", settings.coat);
    setMaterialParam(handle, "coatIor", settings.coatIor);
    setMaterialParam(handle, "coatColor", settings.coatColor);
    setMaterialParam(handle, "coatThickness", settings.coatThickness);
    setMaterialParam(handle, "coatRoughness", settings.coatRoughness);
    setMaterialParam(handle, "coatNormal", settings.coatNormal);
    setMaterialParam(handle, "sheen", settings.sheen);
    setMaterialParam(handle, "sheenColor", settings.sheenColor);
    setMaterialParam(handle, "sheenTint", settings.sheenTint);
    setMaterialParam(handle, "sheenRoughness", settings.sheenRoughness);
    setMaterialParam(handle, "opacity", settings.opacity);
    setMaterialParam(handle, "emissiveColor", settings.emissiveColor);

    commitObject(handle);

    return material;
}
}
