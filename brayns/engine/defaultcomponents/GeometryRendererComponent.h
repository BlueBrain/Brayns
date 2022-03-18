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
#include <brayns/engine/defaultcomponents/MaterialComponent.h>

#include <ospray/ospray.h>

namespace brayns
{
/**
 * @brief Adds renderable geometry to the model
 */
template<typename T>
class GeometryRendererComponent : public Component
{
public:
    GeometryRendererComponent()
    {
        _model = ospNewGeometricModel();
    }

    GeometryRendererComponent(const T& geometry)
    {
        _model = ospNewGeometricModel();
        _geometry.add(geometry);
    }

    GeometryRendererComponent(const std::vector<T>& geometries)
    {
        _model = ospNewGeometricModel();
        _geometry.add(geometries);
    }

    ~GeometryRendererComponent()
    {
        if(_model)
            ospRelease(_model);
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
        _setMaterial();

        auto& model = getModel();
        auto& group = model.getGroup();
        group.addGeometricModel(_model);
    }

    virtual bool commit() override
    {
        if(_geometry.commit())
        {
            auto geometryHandle = _geometry.handle();
            ospSetParam(_model, "geometry", OSPDataType::OSP_GEOMETRY, &geometryHandle);
            ospCommit(_model);
            return true;
        }

        return false;
    }

    virtual void onDestroyed() override
    {
        auto& model = getModel();
        auto& group = model.getGroup();
        group.removeGeometricModel(_model);
        ospRelease(_model);
        _model = nullptr;
    }

protected:
    /**
     * @brief Gives subclasses access to the geometric model
     * @return OSPGeometricModel
     */
    OSPGeometricModel handle() const noexcept
    {
        return _model;
    }

private:
    /**
     * @brief Sets the material object in the geometric model
     */
    void _setMaterial()
    {
        Model& model = getModel();
        MaterialComponent* materialComponent = nullptr;
        try
        {
            auto &componentRef = model.getComponent<MaterialComponent>();
            materialComponent = &componentRef;
        }
        catch(...)
        {
            auto &newComponent = model.addComponent<MaterialComponent>();
            materialComponent = &newComponent;
        }

        auto &material = materialComponent->getMaterial();
        material.commit();
        auto materialHandle = material.handle();
        ospSetParam(_model, "material", OSPDataType::OSP_MATERIAL, &materialHandle);
    }

private:
    OSPGeometricModel _model {nullptr};
    Geometry<T> _geometry;
};
}
