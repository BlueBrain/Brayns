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
#include <brayns/engine/geometries/Isosurface.h>

namespace brayns
{
/**
 * @brief Adds a renderable volume to the model
 */
template<typename T>
class IsosurfaceRendererComponent final : public Component
{
public:
    IsosurfaceRendererComponent(T volumeData, float isovalue)
        : _geometry(Isosurface<T>{Volume<T>(std::move(volumeData)), {isovalue}})
        , _object(_geometry)
    {
        _geometry.commit();
    }

    virtual Bounds computeBounds(const Matrix4f &transform) const noexcept override
    {
        return _geometry.computeBounds(transform);
    }

    virtual void onCreate() override
    {
        Model &model = getModel();

        model.addComponent<MaterialComponent>();

        auto &group = model.getGroup();
        group.addGeometricModel(_object);
    }

    bool commit() override
    {
        Model &group = getModel();
        auto &material = ExtractModelObject::extractMaterial(group);
        if (material.commit())
        {
            _object.setMaterial(material);
            _object.setColor(material.getColor());
            _object.commit();
            return true;
        }

        return false;
    }

    virtual void onDestroy() override
    {
        Model &model = getModel();
        auto &group = model.getGroup();
        group.removeGeometricModel(_object);
    }

private:
    Geometry<Isosurface<T>> _geometry;
    GeometryObject _object;
};
}
