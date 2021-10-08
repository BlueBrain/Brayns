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

#pragma once

#include <brayns/api.h>
#include <brayns/common/PropertyObject.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/mathTypes.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(Material)

namespace brayns
{
typedef std::map<TextureType, Texture2DPtr> TextureDescriptors;

class Material : public PropertyObject
{
public:
    /** @name API for engine-specific code */
    //@{
    /**
     * Called after material change
     */
    virtual void commit() = 0;
    //@}

    BRAYNS_API Material(const PropertyMap& properties = {});

    BRAYNS_API const std::string& getName() const { return _name; }
    BRAYNS_API void setName(const std::string& value)
    {
        _updateValue(_name, value);
    }
    BRAYNS_API void setDiffuseColor(const Vector3d& value)
    {
        _updateValue(_diffuseColor, value);
    }
    BRAYNS_API const Vector3d& getDiffuseColor() const { return _diffuseColor; }
    BRAYNS_API void setSpecularColor(const Vector3d& value)
    {
        _updateValue(_specularColor, value);
    }
    BRAYNS_API const Vector3d& getSpecularColor() const
    {
        return _specularColor;
    }
    BRAYNS_API void setSpecularExponent(double value)
    {
        _updateValue(_specularExponent, value);
    }
    BRAYNS_API double getSpecularExponent() const { return _specularExponent; }
    BRAYNS_API void setReflectionIndex(double value)
    {
        _updateValue(_reflectionIndex, value);
    }
    BRAYNS_API double getReflectionIndex() const { return _reflectionIndex; }
    BRAYNS_API void setOpacity(double value) { _updateValue(_opacity, value); }
    BRAYNS_API double getOpacity() const { return _opacity; }
    BRAYNS_API void setRefractionIndex(double value)
    {
        _updateValue(_refractionIndex, value);
    }
    BRAYNS_API double getRefractionIndex() const { return _refractionIndex; }
    BRAYNS_API void setEmission(double value)
    {
        _updateValue(_emission, value);
    }
    BRAYNS_API double getEmission() const { return _emission; }
    BRAYNS_API void setGlossiness(double value)
    {
        _updateValue(_glossiness, value);
    }
    BRAYNS_API double getGlossiness() const { return _glossiness; }
    BRAYNS_API const TextureDescriptors& getTextureDescriptors() const
    {
        return _textureDescriptors;
    }
    BRAYNS_API void setTexture(const std::string& fileName,
                               const TextureType type);
    BRAYNS_API void removeTexture(const TextureType type);

    BRAYNS_API Texture2DPtr getTexture(const TextureType type) const;
    bool hasTexture(const TextureType type) const
    {
        return _textureDescriptors.count(type) > 0;
    }
    void clearTextures();

protected:
    bool _loadTexture(const std::string& fileName, const TextureType type);

    std::string _name{"undefined"};
    Vector3d _diffuseColor{1., 1., 1.};
    Vector3d _specularColor{1., 1., 1.};
    double _specularExponent{10.};
    double _reflectionIndex{0.};
    double _opacity{1.};
    double _refractionIndex{1.};
    double _emission{0.};
    double _glossiness{1.};
    TexturesMap _textures;
    TextureDescriptors _textureDescriptors;

    SERIALIZATION_FRIEND(Material)
};
} // namespace brayns
