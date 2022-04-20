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

    /**
     * @brief Constructs the component with a single geometry
     * @param geometry
     */
    GeometryRendererComponent(T geometry)
    {
        _geometry.add(std::move(geometry));
    }

    /**
     * @brief Constructs the component with a list of geometries
     * @param geometries
     */
    GeometryRendererComponent(std::vector<T> geometries)
    {
        _geometry.set(std::move(geometries));
    }

    /**
     * @brief Returns the geometry
     * @return
     */
    Geometry<T> &getGeometry() noexcept
    {
        return _geometry;
    }

    /**
     * @brief Sets all the geometry to the same color. Disables material color
     * @param color
     */
    void setColor(const Vector4f &color) noexcept
    {
        GeometricModelHandler::setColor(_model, color);
        _useMaterialColor = false;
        _colorDirty = true;
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

        auto buffer = DataHandler::copyBuffer(colors, OSPDataType::OSP_VEC4F);
        GeometricModelHandler::setColors(_model, buffer);
        _useMaterialColor = false;
        _colorDirty = true;
    }

    /**
     * @brief getSizeInBytes
     * @return
     */
    virtual uint64_t getSizeInBytes() const noexcept override
    {
        return sizeof(GeometryRendererComponent<T>) + _geometry.getNumGeometries() * sizeof(T);
    }

    /**
     * @brief computeBounds
     * @param transform
     * @return
     */
    virtual Bounds computeBounds(const Matrix4f &transform) const noexcept override
    {
        return _geometry.computeBounds(transform);
    }

    /**
     * @brief onStart
     */
    virtual void onStart() override
    {
        Model &model = getModel();
        _model = GeometricModelHandler::create();
        GeometricModelHandler::addToGeometryGroup(_model, model);
        model.addComponent<MaterialComponent>();
    }

    /**
     * @brief commit
     * @return
     */
    virtual bool commit() override
    {
        bool needsCommit = _colorDirty;
        _colorDirty = false;

        Model &model = getModel();

        if (_geometry.commit())
        {
            GeometricModelHandler::setGeometry(_model, _geometry);
            needsCommit = true;
        }

        auto &material = ExtractModelObject::extractMaterial(model);
        if (material.commit())
        {
            GeometricModelHandler::setMaterial(_model, material);
            if (_useMaterialColor)
            {
                GeometricModelHandler::setColor(_model, material.getColor());
            }
            needsCommit = true;
        }

        if (needsCommit)
        {
            GeometricModelHandler::commitModel(_model);
            return true;
        }

        return false;
    }

    /**
     * @brief onDestroyed
     */
    virtual void onDestroyed() override
    {
        GeometricModelHandler::removeFromGeometryGroup(_model, getModel());
        GeometricModelHandler::destory(_model);
    }

private:
    OSPGeometricModel _model{nullptr};
    Geometry<T> _geometry;
    // Need these to flag to avoid using the material color when need to customize colors
    bool _useMaterialColor{true};
    bool _colorDirty{false};
};
}
