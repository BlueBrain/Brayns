/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/Material.h>

#include "Geometry.h"

#include <ospray/ospray_cpp/Data.h>
#include <ospray/ospray_cpp/GeometricModel.h>
#include <ospray/ospray_cpp/Geometry.h>

namespace
{
struct GeometryModelParameters
{
    inline static const std::string material = "material";
    inline static const std::string color = "color";
    inline static const std::string index = "index";
};
}

namespace brayns
{
template<typename T>
class GeometryObject
{
public:
    GeometryObject() = default;

    GeometryObject(T primitive)
        : _geometry(std::move(primitive))
        , _osprayObject(_geometry.getOsprayGeometry())
    {
    }

    GeometryObject(std::vector<T> primitives)
        : _geometry(std::move(primitives))
        , _osprayObject(_geometry.getOsprayGeometry())
    {
    }

    void setMaterial(const Material &material)
    {
        auto &osprayMaterial = material.getOsprayMaterial();
        _osprayObject.setParam(GeometryModelParameters::material, osprayMaterial);
        _modified = true;
    }

    void setColor(const brayns::Vector3f &color)
    {
        setColor(brayns::Vector4f(color, 1.f));
    }

    void setColor(const brayns::Vector4f &color)
    {
        _osprayObject.setParam(GeometryModelParameters::color, color);
        _osprayObject.removeParam(GeometryModelParameters::index);
        _modified = true;
    }

    template<bool Shared>
    void setColorPerPrimitive(const ospray::cpp::Data<Shared> &colors)
    {
        _osprayObject.setParam(GeometryModelParameters::color, colors);
        _osprayObject.removeParam(GeometryModelParameters::index);
        _modified = true;
    }

    template<bool SharedColor, bool SharedIndex>
    void setColorMap(const ospray::cpp::Data<SharedColor> &colors, const ospray::cpp::Data<SharedIndex> &indices)
    {
        _osprayObject.setParam(GeometryModelParameters::color, colors);
        _osprayObject.setParam(GeometryModelParameters::index, indices);
        _modified = true;
    }

    bool commit()
    {
        if (_geometry.commit() || _modified)
        {
            _osprayObject.commit();
            _modified = false;
            return true;
        }
        return false;
    }

    Bounds computeBounds(const Matrix4f &transform) const noexcept
    {
        return _geometry.computeBounds(transform);
    }

    Geometry<T> &getGeometry() noexcept
    {
        return _geometry;
    }

    size_t getNumPrimitives() const noexcept
    {
        return _geometry.getPrimitives().size();
    }

    const ospray::cpp::GeometricModel &getOsprayObject() const noexcept
    {
        return _osprayObject;
    }

private:
    Geometry<T> _geometry;
    ospray::cpp::GeometricModel _osprayObject;
    bool _modified = true;
};
}
