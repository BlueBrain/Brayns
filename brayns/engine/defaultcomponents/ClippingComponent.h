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

#include <brayns/engine/Geometry.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/ModelComponents.h>

#include <ospray/ospray.h>

namespace brayns
{
/**
 * @brief Adds clipping geometry to the model (Affects the whole scene)
 */
template<typename T>
class ClippingComponent : public Component
{
public:
    ClippingComponent(const T& geometry)
    {
        _initializeHandle();
        _geometry.add(geometry);
    }

    ~ClippingComponent()
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
        return sizeof(ClippingComponent<T>) + _geometry.getNumGeometries() * sizeof(T);
    }

    virtual void onStart() override
    {
        auto& model = getModel();
        auto& group = model.getGroup();
        group.addClippingModel(_model);
    }

    virtual void onCommit() override
    {
        bool needsCommit = false;

        needsCommit = needsCommit || _commitGeometry();

        if(needsCommit)
        {
            ospCommit(_model);
        }
    }

    virtual void onDestroyed() override
    {
        auto& model = getModel();
        auto& group = model.getGroup();
        group.removeClippingModel(_model);
        ospRelease(_model);
        _model = nullptr;
    }

protected:
    OSPGeometricModel handle() const noexcept
    {
        return _model;
    }

private:
    void _initializeHandle()
    {
        _model = ospNewGeometricModel();
    }

    bool _commitGeometry()
    {
        if(_geometry.isModified())
        {
            _geometry.doCommit();
            auto geometryHandle = _geometry.handle();
            ospSetParam(_model, "geometry", OSPDataType::OSP_GEOMETRY, &geometryHandle);
            return true;
        }

        return false;
    }

private:
    OSPGeometricModel _model {nullptr};
    Geometry<T> _geometry;
};
}
