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
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/components/MaterialComponent.h>
#include <brayns/engine/geometries/Isosurface.h>

#include <ospray/ospray.h>

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
        : _model(ospNewGeometricModel())
    {
        Isosurface<T> isosurface;
        auto &volume = isosurface.volume;
        volume.setData(std::move(volumeData));
        volume.commit();
        isosurface.isovalues.push_back(isovalue);

        _geometry.add(std::move(isosurface));
        _geometry.commit();

        GeometricModelHandler::setGeometry(_model, _geometry);
    }

    virtual Bounds computeBounds(const Matrix4f &transform) const noexcept override
    {
        return _geometry.computeBounds(transform);
    }

    virtual void onCreate() override
    {
        Model &group = getModel();
        auto &component = group.addComponent<MaterialComponent>();
        auto &material = component.getMaterial();
        material.setColor(brayns::Vector3f(0.5f));
        GeometricModelHandler::addToGeometryGroup(_model, group);
    }

    bool commit() override
    {
        Model &group = getModel();
        auto &material = ExtractModelObject::extractMaterial(group);
        if (material.commit())
        {
            GeometricModelHandler::setMaterial(_model, material);
            GeometricModelHandler::setColor(_model, material.getColor());
            GeometricModelHandler::commitModel(_model);
            return true;
        }

        return false;
    }

    virtual void onDestroy() override
    {
        auto &model = getModel();
        GeometricModelHandler::removeFromGeometryGroup(_model, model);
        GeometricModelHandler::destroy(_model);
    }

private:
    OSPGeometricModel _model = nullptr;
    Geometry<Isosurface<T>> _geometry;
};
}
