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
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/mathTypes.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(Material)

namespace brayns
{
typedef std::map<TextureType, std::string> TextureTypes;

class Material
{
public:
    Material();
    BRAYNS_API MaterialType getType() const { return _materialType; }
    BRAYNS_API void setType(const MaterialType materialType)
    {
        _materialType = materialType;
    }
    BRAYNS_API void setColor(const Vector3f& value) { _color = value; }
    BRAYNS_API Vector3f& getColor() { return _color; }
    BRAYNS_API void setSpecularColor(const Vector3f& value)
    {
        _specularColor = value;
    }
    BRAYNS_API Vector3f& getSpecularColor() { return _specularColor; }
    BRAYNS_API void setSpecularExponent(float value)
    {
        _specularExponent = value;
    }
    BRAYNS_API float getSpecularExponent() const { return _specularExponent; }
    BRAYNS_API void setReflectionIndex(float value)
    {
        _reflectionIndex = value;
    }
    BRAYNS_API float getReflectionIndex() const { return _reflectionIndex; }
    BRAYNS_API void setOpacity(float value) { _opacity = value; }
    BRAYNS_API float getOpacity() const { return _opacity; }
    BRAYNS_API void setRefractionIndex(float value)
    {
        _refractionIndex = value;
    }
    BRAYNS_API float getRefractionIndex() const { return _refractionIndex; }
    BRAYNS_API void setEmission(float value) { _emission = value; }
    BRAYNS_API float getEmission() const { return _emission; }
    BRAYNS_API void setGlossiness(float value) { _glossiness = value; }
    BRAYNS_API float getGlossiness() const { return _glossiness; }
    BRAYNS_API void setCastSimulationData(bool value)
    {
        _castSimulationData = value;
    }
    BRAYNS_API bool getCastSimulationData() const
    {
        return _castSimulationData;
    }
    BRAYNS_API TextureTypes& getTextures() { return _textures; }
    BRAYNS_API void setTexture(const TextureType& type,
                               const std::string& filename);

    /* @brief Prevents material attributes to be updated. Any update will be
     * ignored. This is used when one wants to avoid material attibutes to be
     * modifiied during the life time of the scene
    */
    BRAYNS_API void lock() { _locked = true; }
    /* @brief Allows material attributes to be modified */
    BRAYNS_API void unlock() { _locked = false; }
    /* @return True if material attributes can be updated, false otherwise */
    BRAYNS_API bool locked() const { return _locked; }
private:
    MaterialType _materialType;
    Vector3f _color;
    Vector3f _specularColor;
    float _specularExponent;
    float _reflectionIndex;
    float _opacity;
    float _refractionIndex;
    float _emission;
    float _glossiness;
    bool _castSimulationData;
    TextureTypes _textures;
    bool _locked;

    SERIALIZATION_FRIEND(Material)
};
}
#endif // MATERIAL_H
