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

#include <brayns/engine/RenderableType.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/ExtractModelObject.h>
#include <brayns/engine/common/GeometricModelHandler.h>
#include <brayns/engine/components/MaterialComponent.h>

#include <ospray/ospray.h>

namespace brayns
{
/**
 * @brief Adds renderable geometry to the model
 */
template<typename T>
class GeometryRendererComponent final : public Component
{
public:
    GeometryRendererComponent() = default;

    GeometryRendererComponent(const T& geometry)
    {
        _geometry.add(geometry);
    }

    GeometryRendererComponent(std::vector<T> geometries)
    {
        _geometry.set(std::move(geometries));
    }

    Geometry<T> &getGeometry() noexcept
    {
        return _geometry;
    }

    virtual uint64_t getSizeInBytes() const noexcept override
    {
        return sizeof(GeometryRendererComponent<T>) + _geometry.getNumGeometries() * sizeof(T);
    }

    virtual Bounds computeBounds(const Matrix4f& transform) const noexcept override
    {
        return _geometry.computeBounds(transform);
    }

    virtual void onStart() override
    {
        Model &model = getModel();
        _model = GeometricModelHandler::create();
        GeometricModelHandler::addToGeometryGroup(_model, model);
        model.addComponent<MaterialComponent>();
    }

    virtual bool commit() override
    {
        bool needsCommit = false;
        Model &model = getModel();

        if(_geometry.commit())
        {
            GeometricModelHandler::setGeometry(_model, _geometry);
            needsCommit = true;
        }

        auto &material = ExtractModelObject::extractMaterial(model);
        if(material.commit())
        {
            GeometricModelHandler::setMaterial(_model, material);
            GeometricModelHandler::setColor(_model, material.getColor());
            needsCommit = true;
        }

        if(needsCommit)
        {
            GeometricModelHandler::commitModel(_model);
            return true;
        }

        return false;
    }

    virtual void onDestroyed() override
    {
        GeometricModelHandler::removeFromGeometryGroup(_model, getModel());
        GeometricModelHandler::destory(_model);
    }

private:
    OSPGeometricModel _model {nullptr};
    Geometry<T> _geometry;
};
}
