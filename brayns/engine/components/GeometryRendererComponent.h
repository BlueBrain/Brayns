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

#include <brayns/engine/GeometryObject.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/components/MaterialComponent.h>

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
    GeometryRendererComponent(T geometry)
        : _geometry(std::move(geometry))
        , _object(_geometry)
    {
    }

    /**
     * @brief Constructs the component with a list of geometries
     * @param geometries
     */
    GeometryRendererComponent(std::vector<T> geometries)
        : _geometry(std::move(geometries))
        , _object(_geometry)
    {
    }

    /**
     * @brief Returns a modifiable geometry reference
     * @return Geometry<T> &
     */
    Geometry<T> &getGeometry() noexcept
    {
        return _geometry;
    }

    /**
     * @brief Sets a color per geometry. Disables material color
     * @param colors
     * @throws std::invalid_argument if there are not enough colors for the contained geometry
     */
    void setColors(const std::vector<brayns::Vector4f> &colors)
    {
        if (colors.size() < _geometry.getNumGeometries())
        {
            throw std::invalid_argument("Not enough colors for all geometry");
        }

        auto buffer = ospray::cpp::CopiedData(colors);
        _object.setColorPerPrimitive(buffer.handle());
        _useMaterialColor = false;
        _colorDirty = true;
    }

    virtual Bounds computeBounds(const Matrix4f &transform) const noexcept override
    {
        return _geometry.computeBounds(transform);
    }

    virtual void onCreate() override
    {
        Model &model = getModel();

        auto &group = model.getGroup();
        group.addGeometricModel(_object);

        model.addComponent<MaterialComponent>();
    }

    virtual bool commit() override
    {
        bool needsCommit = _colorDirty;
        _colorDirty = false;

        auto &material = ExtractModelObject::extractMaterial(getModel());
        if (material.commit())
        {
            needsCommit = true;
            _object.setMaterial(material);
            if (_useMaterialColor)
            {
                _object.setColor(material.getColor());
            }
        }

        if (_geometry.commit())
        {
            needsCommit = true;
        }

        if (needsCommit)
        {
            _object.commit();
        }

        return needsCommit;
    }

    virtual void onDestroy() override
    {
        Model &model = getModel();
        auto &group = model.getGroup();
        group.removeGeometricModel(_object);
    }

private:
    Geometry<T> _geometry;
    GeometryObject _object;
    // Need these to flag to avoid using the material color when need to customize colors
    bool _useMaterialColor{true};
    bool _colorDirty{false};
};
}
