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

typedef std::map<TextureType, Texture2DPtr> TextureDescriptors;

class Material : public BaseObject
{
public:
    /**
     * Called after material change
     */
    BRAYNS_API virtual void commit() = 0;

    BRAYNS_API const std::string& getName() const { return _name; }
    BRAYNS_API void setName(const std::string& value)
    {
        _updateValue(_name, value);
    }
    BRAYNS_API void setDiffuseColor(const Vector3f& value)
    {
        _updateValue(_diffuseColor, value);
    }
    BRAYNS_API const Vector3f& getDiffuseColor() const { return _diffuseColor; }
    BRAYNS_API void setSpecularColor(const Vector3f& value)
    {
        _updateValue(_specularColor, value);
    }
    BRAYNS_API const Vector3f& getSpecularColor() const
    {
        return _specularColor;
    }
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
    BRAYNS_API const TextureDescriptors& getTextureDescriptors() const
    {
        return _textureDescriptors;
    }
    BRAYNS_API void setTexture(const std::string& fileName,
                               const TextureType& type);

    BRAYNS_API Texture2DPtr getTexture(const TextureType& type) const;

protected:
    bool _loadTexture(const std::string& fileName);

    std::string _name{"undefined"};
    Vector3f _diffuseColor{1.f, 1.f, 1.f};
    Vector3f _specularColor{1.f, 1.f, 1.f};
    float _specularExponent{10.f};
    float _reflectionIndex{0.f};
    float _opacity{1.f};
    float _refractionIndex{1.f};
    float _emission{0.f};
    float _glossiness{1.f};
    bool _castSimulationData{true};
    TexturesMap _textures;
    TextureDescriptors _textureDescriptors;

    SERIALIZATION_FRIEND(Material)
};
}
#endif // MATERIAL_H
