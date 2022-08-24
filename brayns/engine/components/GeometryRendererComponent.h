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

#include <brayns/engine/common/MathTypesOsprayTraits.h>
#include <brayns/engine/components/MaterialComponent.h>
#include <brayns/engine/geometry/GeometryView.h>
#include <brayns/engine/model/Model.h>
#include <brayns/engine/model/ModelComponents.h>

#include <ospray/ospray_cpp/Data.h>

namespace brayns
{
/**
 * @brief Adds renderable geometry to the model
 */
class GeometryRendererComponent final : public Component
{
public:
    template<typename T>
    GeometryRendererComponent(T primitive)
        : GeometryRendererComponent(std::vector<T>{std::move(primitive)})
    {
    }

    template<typename T>
    GeometryRendererComponent(std::vector<T> primitives)
        : _geometry(std::move(primitives))
        , _geometryView(_geometry)
    {
    }

    /**
     * @brief Returns a modifiable geometry reference
     * @return Geometry<T> &
     */
    Geometry &getGeometry() noexcept;

    /**
     * @brief Sets a color per geometry. Disables material color
     * @param colors
     * @throws std::invalid_argument if there are not enough colors for the contained geometry
     */
    void setColors(const std::vector<brayns::Vector4f> &colors);

    virtual Bounds computeBounds(const Matrix4f &transform) const noexcept override;
    virtual void onCreate() override;
    virtual bool commit() override;

private:
    Geometry _geometry;
    GeometryView _geometryView;
    bool _useMaterialColor = true;
};
}
