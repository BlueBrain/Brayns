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
#include <brayns/engine/RenderableType.h>
#include <brayns/engine/common/GeometricModelHandler.h>

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
        _model = GeometricModelHandler::create();
        _geometry.add(geometry);
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
        GeometricModelHandler::addToClippingGroup(_model, getModel());
    }

    virtual bool commit() override
    {
        if(_geometry.commit())
        {
            GeometricModelHandler::setGeometry(_model, _geometry);
            GeometricModelHandler::commitModel(_model);
            return true;
        }

        return false;
    }

    virtual void onDestroyed() override
    {
        GeometricModelHandler::removeFromClippingGroup(_model, getModel());
        GeometricModelHandler::destory(_model);
    }

private:
    OSPGeometricModel _model {nullptr};
    Geometry<T> _geometry;
};
}
