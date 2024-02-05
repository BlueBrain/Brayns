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

#pragma once

#include <brayns/common/MathTypes.h>
#include <brayns/common/PropertyObject.h>

#include <memory>

namespace brayns
{
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

    Material(const PropertyMap& properties = {});

    const std::string& getName() const { return _name; }
    void setName(const std::string& value) { _updateValue(_name, value); }
    void setDiffuseColor(const Vector3d& value)
    {
        _updateValue(_diffuseColor, value);
    }
    const Vector3d& getDiffuseColor() const { return _diffuseColor; }
    void setSpecularColor(const Vector3d& value)
    {
        _updateValue(_specularColor, value);
    }
    const Vector3d& getSpecularColor() const { return _specularColor; }
    void setSpecularExponent(double value)
    {
        _updateValue(_specularExponent, value);
    }
    double getSpecularExponent() const { return _specularExponent; }
    void setReflectionIndex(double value)
    {
        _updateValue(_reflectionIndex, value);
    }
    double getReflectionIndex() const { return _reflectionIndex; }
    void setOpacity(double value) { _updateValue(_opacity, value); }
    double getOpacity() const { return _opacity; }
    void setRefractionIndex(double value)
    {
        _updateValue(_refractionIndex, value);
    }
    double getRefractionIndex() const { return _refractionIndex; }
    void setEmission(double value) { _updateValue(_emission, value); }
    double getEmission() const { return _emission; }
    void setGlossiness(double value) { _updateValue(_glossiness, value); }
    double getGlossiness() const { return _glossiness; }

protected:
    std::string _name{"undefined"};
    Vector3d _diffuseColor{1., 1., 1.};
    Vector3d _specularColor{1., 1., 1.};
    double _specularExponent{10.};
    double _reflectionIndex{0.};
    double _opacity{1.};
    double _refractionIndex{1.};
    double _emission{0.};
    double _glossiness{1.};
};

using MaterialPtr = std::shared_ptr<Material>;

} // namespace brayns
