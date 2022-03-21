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
        bool needsCommit = false;
        if(_geometry.commit())
        {
            auto geometryHandle = _geometry.handle();
            ospSetParam(_model, "geometry", OSPDataType::OSP_GEOMETRY, &geometryHandle);
            //ospCommit(_model);
            //return true;
            needsCommit = true;
        }

        auto &material = _materialComponent->getMaterial();
        if(material.commit())
        {
            auto materialHandle = material.handle();
            ospSetParam(_model, "material", OSPDataType::OSP_MATERIAL, &materialHandle);
            commitColor();
            needsCommit = true;
        }

        if(needsCommit)
        {
            ospCommit(_model);
        }

        return needsCommit;
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

    /**
     * @brief specifies how colors are committed into the Geometric Model.
     * There are 3 options:
     * - Material base color
     * - List of colors, 1 per primitive
     * - Indexed list of colors, with 1 index per primitive
     * 
     * OSPRay will always take as base color the material one, and, if any other
     * color (from the list or from the indexed list) is present, it will blend them
     * together. 
     * 
     * To be able to have full control of the colors, all materials use as base color
     * full white (1, 1, 1). Then, by default, GeometryRendererComponent will commit the
     * material base color as a single color that will be used by the underneath geometry
     * (All geometry will share the same color).
     * 
     * Subclasses of the GeometryRendererComponent may override this method to change this
     * behaviour (An example can be seen in brayns/io/loaders/ProteinRendererComponent)
     * 
     */
    virtual void commitColor()
    {
        auto &material = _materialComponent->getMaterial();
        const auto &color = material.getColor();

        // Blending the material opacity with color opacity = material opacity
        Vector4f finalColor (color, 1.f);
        ospSetParam(_model, "color", OSPDataType::OSP_VEC4F, &finalColor);
    }

private:
    /**
     * @brief Sets the material object in the geometric model
     */
    void _setMaterial()
    {
        Model& model = getModel();
        try
        {
            auto &componentRef = model.getComponent<MaterialComponent>();
            _materialComponent = &componentRef;
        }
        catch(...)
        {
            auto &newComponent = model.addComponent<MaterialComponent>();
            _materialComponent = &newComponent;
        }
    }

private:
    OSPGeometricModel _model {nullptr};
    Geometry<T> _geometry;
    MaterialComponent *_materialComponent {nullptr};
};
}
