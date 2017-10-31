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

namespace brayns
{
typedef std::map<TextureType, std::string> TextureTypes;

class Material
{
public:
    Material();

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
    BRAYNS_API TextureTypes& getTextures() { return _textures; }
    BRAYNS_API void setTexture(const TextureType& type,
                               const std::string& filename);
    BRAYNS_API void lock() { _locked = true; }
    BRAYNS_API void unlock() { _locked = false; }
    BRAYNS_API bool locked() { return _locked; }
private:
    Vector3f _color;
    Vector3f _specularColor;
    float _specularExponent;
    float _reflectionIndex;
    float _opacity;
    float _refractionIndex;
    float _emission;
    float _glossiness;
    TextureTypes _textures;
    bool _locked;
};
}
#endif // MATERIAL_H
