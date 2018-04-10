/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <brayns/api.h>
#include <brayns/common/BaseObject.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/mathTypes.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(Material)
SERIALIZATION_ACCESS(TextureDescriptor)

namespace brayns
{
enum TextureType
{
    TT_DIFFUSE = 0,
    TT_NORMALS,
    TT_BUMP,
    TT_SPECULAR,
    TT_EMISSIVE,
    TT_OPACITY,
    TT_REFLECTION,
    TT_REFRACTION,
    TT_OCCLUSION
};

struct TextureTypeMaterialAttribute
{
    TextureType type;
    std::string attribute;
};

static TextureTypeMaterialAttribute textureTypeMaterialAttribute[7] = {
    {TT_DIFFUSE, "map_kd"},
    {TT_NORMALS, "map_bump"},
    {TT_SPECULAR, "map_ks"},
    {TT_EMISSIVE, "map_a"},
    {TT_OPACITY, "map_d"},
    {TT_REFLECTION, "map_reflection"},
    {TT_REFRACTION, "map_refraction"}};

class TextureDescriptor : public BaseObject
{
public:
    BRAYNS_API TextureDescriptor();
    BRAYNS_API TextureDescriptor(const TextureType type, const size_t id);
    BRAYNS_API TextureDescriptor(const TextureDescriptor& rhs);
    BRAYNS_API TextureDescriptor& operator=(const TextureDescriptor& rhs);

    BRAYNS_API size_t getId() const { return _id; }
    BRAYNS_API TextureType getType() const { return _type; }
private:
    TextureType _type;
    size_t _id;

    SERIALIZATION_FRIEND(TextureDescriptor)
};
typedef std::vector<TextureDescriptor> TextureDescriptors;

class Material : public BaseObject
{
public:
    BRAYNS_API Material();
    BRAYNS_API Material(const Material& rhs);
    BRAYNS_API Material& operator=(const Material& rhs);

    BRAYNS_API std::string getName() const { return _name; }
    BRAYNS_API void setName(const std::string& value)
    {
        _updateValue(_name, value);
    }
    BRAYNS_API void setDiffuseColor(const Vector3f& value)
    {
        _updateValue(_color, value);
    }
    BRAYNS_API Vector3f& getDiffuseColor() { return _color; }
    BRAYNS_API void setSpecularColor(const Vector3f& value)
    {
        _updateValue(_specularColor, value);
    }
    BRAYNS_API Vector3f& getSpecularColor() { return _specularColor; }
    BRAYNS_API void setSpecularExponent(float value)
    {
        _updateValue(_specularExponent, value);
    }
    BRAYNS_API float getSpecularExponent() const { return _specularExponent; }
    BRAYNS_API void setReflectionIndex(float value)
    {
        _updateValue(_reflectionIndex, value);
    }
    BRAYNS_API float getReflectionIndex() const { return _reflectionIndex; }
    BRAYNS_API void setOpacity(float value) { _updateValue(_opacity, value); }
    BRAYNS_API float getOpacity() const { return _opacity; }
    BRAYNS_API void setRefractionIndex(float value)
    {
        _updateValue(_refractionIndex, value);
    }
    BRAYNS_API float getRefractionIndex() const { return _refractionIndex; }
    BRAYNS_API void setEmission(float value) { _updateValue(_emission, value); }
    BRAYNS_API float getEmission() const { return _emission; }
    BRAYNS_API void setGlossiness(float value)
    {
        _updateValue(_glossiness, value);
    }
    BRAYNS_API float getGlossiness() const { return _glossiness; }
    BRAYNS_API void setCastSimulationData(bool value)
    {
        _updateValue(_castSimulationData, value);
    }
    BRAYNS_API bool getCastSimulationData() const
    {
        return _castSimulationData;
    }
    BRAYNS_API TextureDescriptors& getTextureDescriptors()
    {
        return _textureDescriptors;
    }
    BRAYNS_API void addTexture(const TextureType& type, const size_t id);

protected:
    std::string _name{"undefined"};
    Vector3f _color{1.f, 1.f, 1.f};
    Vector3f _specularColor{1.f, 1.f, 1.f};
    float _specularExponent{10.f};
    float _reflectionIndex{0.f};
    float _opacity{1.f};
    float _refractionIndex{1.f};
    float _emission{0.f};
    float _glossiness{1.f};
    bool _castSimulationData{true};
    TextureDescriptors _textureDescriptors;

    SERIALIZATION_FRIEND(Material)
};
}
#endif // MATERIAL_H
