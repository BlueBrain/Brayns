/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/Model.h>
#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>
#include <brayns/engine/geometry/GeometryObject.h>

#include <ospray/ospray_cpp/Data.h>

namespace brayns
{
/**
 * @brief Adds renderable geometry to the model
 */
template<typename T>
class GeometryRendererComponent final : public Component
{
public:
    /**
     * @brief Constructs the component with a single geometry
     * @param geometry
     */
    GeometryRendererComponent(T primitive)
        : _geometryObject(std::move(primitive))
    {
    }

    /**
     * @brief Constructs the component with a list of geometries
     * @param geometries
     */
    GeometryRendererComponent(std::vector<T> primitives)
        : _geometryObject(std::move(primitives))
    {
    }

    /**
     * @brief Returns a modifiable geometry reference
     * @return Geometry<T> &
     */
    Geometry<T> &getGeometry() noexcept
    {
        return _geometryObject.getGeometry();
    }

    /**
     * @brief Sets a color per geometry. Disables material color
     * @param colors
     * @throws std::invalid_argument if there are not enough colors for the contained geometry
     */
    void setColors(const std::vector<brayns::Vector4f> &colors)
    {
        auto &geometry = _geometryObject.getGeometry();
        if (colors.size() < geometry.getPrimitives().size())
        {
            throw std::invalid_argument("Not enough colors for all geometry");
        }

        _geometryObject.setColorPerPrimitive(ospray::cpp::CopiedData(colors));
        _useMaterialColor = false;
    }

    virtual Bounds computeBounds(const Matrix4f &transform) const noexcept override
    {
        return _geometryObject.computeBounds(transform);
    }

    virtual void onCreate() override
    {
        Model &model = getModel();

        auto &group = model.getGroup();
        group.addGeometry(_geometryObject);

        model.addComponent<MaterialComponent>();
    }

    virtual bool commit() override
    {
        auto &material = ExtractModelObject::extractMaterial(getModel());
        if (material.commit())
        {
            _geometryObject.setMaterial(material);
            if (_useMaterialColor)
            {
                _geometryObject.setColor(material.getColor());
            }
        }

        return _geometryObject.commit();
    }

    virtual void onDestroy() override
    {
        Model &model = getModel();
        auto &group = model.getGroup();
        group.removeGeometry(_geometryObject);
    }

private:
    GeometryObject<T> _geometryObject;
    bool _useMaterialColor = true;
};
}
