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
#include <brayns/engine/defaultcomponents/GeometryComponent.h>
#include <brayns/engine/defaultcomponents/MaterialComponent.h>

#include <ospray/ospray.h>

namespace brayns
{
template<typename T>
class GeometryRendererComponent : public Component
{
public:
    GeometryRendererComponent()
    {
        _model = ospNewGeometricModel();
    }

    ~GeometryRendererComponent()
    {
        if(_model)
            ospRelease(_model);
    }

    uint64_t getSizeInBytes() const noexcept override
    {
        return sizeof(GeometryRendererComponent<T>);
    }

    void onStart() override
    {
        auto& model = getModel();
        auto& group = model.getGroup();
        group.addGeometricModel(_model);
    }

    void onCommit() override
    {
        Model& model = getModel();

        auto& geomComponent = model.getComponent<GeometryComponent<T>>();
        auto& geometry = geomComponent.getGeometry();
        auto& materialComponent = model.getComponent<MaterialComponent>();
        auto& material = materialComponent.getMaterial();

        if(!geometry.isModified() && !material.isModified())
        {
            return;
        }

        geometry.doCommit();
        material.doCommit();

        auto geometryHandle = geometry.handle();
        auto materialHandle = material.handle();

        ospSetParam(_model, "geometry", OSPDataType::OSP_GEOMETRY, &geometryHandle);
        ospSetParam(_model, "material", OSPDataType::OSP_MATERIAL, &materialHandle);

        ospCommit(_model);
    }

    void onDestroyed() override
    {
        auto& model = getModel();
        auto& group = model.getGroup();
        group.removeGeometricModel(_model);
        ospRelease(_model);
        _model = nullptr;
    }

protected:
    OSPGeometricModel handle() const noexcept
    {
        return _model;
    }

private:
    OSPGeometricModel _model {nullptr};
};
}
