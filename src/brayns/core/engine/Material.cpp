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

void setTextureParams(OSPMaterial handle, const std::string &id, const MaterialTexture2D &texture)
{
    setObjectParam(handle, id.c_str(), texture.value);

    auto translation = id + ".translation";
    setObjectParam(handle, translation.c_str(), texture.transform.translation);

    auto rotation = id + ".rotation";
    setObjectParam(handle, rotation.c_str(), degrees(texture.transform.rotation));

    auto scale = id + ".scale";
    setObjectParam(handle, scale.c_str(), texture.transform.scale);
}

void setTextureParams(OSPMaterial handle, const std::string &id, const MaterialVolumeTexture &texture)
{
    setObjectParam(handle, id.c_str(), texture.value);

    auto transform = id + ".transform";
    setObjectParam(handle, transform.c_str(), toAffine(texture.transform));
}

template<typename T, OsprayDataType U>
void setMaterialParam(OSPMaterial handle, const char *id, const MaterialTexture<T, U> &texture)
{
    auto textureId = std::string("map_") + id;
    setTextureParams(handle, textureId, texture.value);

    setObjectParam(handle, id, texture.factor);
}

template<OsprayDataType T>
void setMaterialParam(OSPMaterial handle, const char *id, const T &factor)
{
    setObjectParam(handle, id, factor);
}

template<typename T>
void setMaterialParam(OSPMaterial handle, const char *id, const MaterialField<T> &field)
{
    std::visit([&](const auto &value) { setMaterialParam(handle, id, value); }, field);
}

void setAoMaterialParams(OSPMaterial handle, const AoMaterialSettings &settings)
{
    setMaterialParam(handle, "kd", settings.diffuse);
    setMaterialParam(handle, "d", settings.opacity);
}

void setScivisMaterialParams(OSPMaterial handle, const ScivisMaterialSettings &settings)
{
    setMaterialParam(handle, "kd", settings.diffuse);
    setMaterialParam(handle, "d", settings.opacity);
    setMaterialParam(handle, "ks", settings.specular);
    setMaterialParam(handle, "ns", settings.shininess);
    setObjectParam(handle, "tf", settings.transparencyFilter);
}

void setPrincipledMaterialParams(OSPMaterial handle, const PrincipledMaterialSettings &settings)
{
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
    setObjectParam(handle, "thin", settings.thin);
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
}
}

namespace brayns
{
void AoMaterial::update(const AoMaterialSettings &settings)
{
    auto handle = getHandle();
    setAoMaterialParams(handle, settings);
    commitObject(handle);
}

AoMaterial createAoMaterial(Device &device, const AoMaterialSettings &settings)
{
    auto handle = ospNewMaterial("obj");
    auto material = wrapObjectHandleAs<AoMaterial>(device, handle);

    setAoMaterialParams(handle, settings);

    commitObject(device, handle);

    return material;
}

void ScivisMaterial::update(const ScivisMaterialSettings &settings)
{
    auto handle = getHandle();
    setScivisMaterialParams(handle, settings);
    commitObject(handle);
}

ScivisMaterial createScivisMaterial(Device &device, const ScivisMaterialSettings &settings)
{
    auto handle = ospNewMaterial("obj");
    auto material = wrapObjectHandleAs<ScivisMaterial>(device, handle);

    setScivisMaterialParams(handle, settings);

    commitObject(device, handle);

    return material;
}

void PrincipledMaterial::update(const PrincipledMaterialSettings &settings)
{
    auto handle = getHandle();
    setPrincipledMaterialParams(handle, settings);
    commitObject(handle);
}

PrincipledMaterial createPrincipledMaterial(Device &device, const PrincipledMaterialSettings &settings)
{
    auto handle = ospNewMaterial("principled");
    auto material = wrapObjectHandleAs<PrincipledMaterial>(device, handle);

    setPrincipledMaterialParams(handle, settings);

    commitObject(device, handle);

    return material;
}
}
