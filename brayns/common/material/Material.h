/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <brayns/common/mathTypes.h>
#include <brayns/common/material/Texture2D.h>

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
    { _specularColor = value; }
    BRAYNS_API Vector3f& getSpecularColor() { return _specularColor; }

    BRAYNS_API void setSpecularExponent(float value)
    { _specularExponent = value; }
    BRAYNS_API float getSpecularExponent() const { return _specularExponent; }

    BRAYNS_API void setReflectionIndex(float value)
    { _reflectionIndex = value; }
    BRAYNS_API float getReflectionIndex() const { return _reflectionIndex; }

    BRAYNS_API void setOpacity(float value) { _opacity = value; }
    BRAYNS_API float getOpacity() const { return _opacity; }

    BRAYNS_API void setRefractionIndex(float value) { _refractionIndex=value; }
    BRAYNS_API float getRefractionIndex() const { return _refractionIndex; }

    BRAYNS_API void setEmission(float value) { _emission=value; }
    BRAYNS_API float getEmission() const { return _emission; }

    BRAYNS_API TextureTypes& getTextures() { return _textures; }
    BRAYNS_API void setTexture( const TextureType& type,
                                const std::string& filename );

private:
    Vector3f _color;
    Vector3f _specularColor;
    float _specularExponent;
    float _reflectionIndex;
    float _opacity;
    float _refractionIndex;
    float _emission;
    TextureTypes _textures;
};

}
#endif // MATERIAL_H
