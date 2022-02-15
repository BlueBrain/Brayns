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

#include <brayns/common/MathTypes.h>
#include <brayns/engine/Geometry.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/materials/MatteMaterial.h>

namespace brayns
{
/**
 * @brief The GeometricModel class is an implementation of the Model class with the capabilities of handling generic
 * geometry efficiently. For a more specific use-case, it my be worth to implement directly a custom model.
 *
 * It allows for a single material for all the geometry, but allows to specify color and opacity on a per-geometry
 * level, and as a color map with per-geometry indices into the color map level. If none specified, it will apply
 * the material base color to all the geometries.
 */
template<typename T>
class GeometricModel : public Model
{
public:
    GeometricModel(T&& geometry)
    {
        _initialize();
        _geometry.add(geometry);
    }

    GeometricModel(const std::vector<T>& geometries)
    {
        _initialize();
        _geometry.add(geometries);
    }

    ~GeometricModel()
    {
        ospRelease(_handle);
    }

    /**
     * @brief Sets the material applied to the surface of the geometry handled by this model
     */
    void setMaterial(Material::Ptr&& material)
    {
        if(!material)
            throw std::invalid_argument("GeometricModel: Null material is not allowed");

        _material = std::move(material);
        _material->markModified();
        markModified(false);
    }

    /**
     * @brief Returns the material of this model
     */
    const Material& getMaterial() const noexcept
    {
        return *_material;
    }

    /**
     * @brief Commit implementation
     */
    void commit() override
    {
        if(_geometry->isModified())
        {
            _geometry->doCommit();

            auto geometryHandle = _geometry->handle();
            ospSetParam(_handle, "geometry", OSP_GEOMETRY, &geometryHandle);
        }

        if(_material->isModified())
        {
            _material->doCommit();

            auto materialHandle = _material->handle();
            ospSetParam(_handle, "material", OSP_MATERIAL, &materialHandle);
        }

        if(_colorsDirty)
        {
            auto colorData = ospNewSharedData(_colors.data(), OSPDataType::OSP_VEC4F, _colors.size());
            ospSetParam(_handle, "color", OSPDataType::OSP_DATA, &colorData);
            ospRelease(colorData);
            _colorsDirty = false;
        }

        if(_colorIndicesDirty)
        {
            auto colorIndexData = ospNewSharedData(_colorIndices.data(), OSPDataType::OSP_UCHAR, _colorIndices.size());
            ospSetParam(_handle, "index", OSP_DATA, &colorIndexData);
            ospRelease(colorIndexData);
            _colorIndicesDirty = false;
        }

        ospCommit(_handle);
    }

    uint32_t getNumGeometries() const noexcept
    {
        return _geometry.getNumGeometries();
    }

    /**
     * @brief Sets the colors of this geometry, one per geometry
     */
    void setColors(std::vector<Vector4f>&& colors) noexcept
    {
        if(colors.size() < _geometry.getNumGeometries())
            throw std::invalid_argument("When specifying colors only, there must be 1 color per geometry");

        _colors = std::move(colors);
        _colorsDirty = true;
        markModified(false);
    }

    /**
     * @brief Sets the colors of this geometry as a color map, with an indices buffer to access these colors.
     * There must be a maximum of 256 colorso nthe colors buffer.
     * There must be one entry on the color indices per geometry.
     * @param colors color map with a maximum of 256 colors
     * @param colorIndices color indices with an entry per geometry
     */
    void setColors(std::vector<Vector4f>&& colors, std::vector<uint8_t>&& colorIndices) noexcept
    {
        if(colorIndices.size() < _geometry.getNumGeometries())
            throw std::invalid_argument("When specifying color map, there must be 1 color index per geometry");

        if(colors.size() > 256)
            Log::warn("GeometicModel: Adding a color map with more than 256 colors");

        const uint8_t size = colors.size() > 256? 255 : static_cast<uint8_t>(colors.size() - 1);

        for(const auto index : colorIndices)
        {
            if(index > size)
                throw std::invalid_argument("GeometricModel: Color map indices are beyond the color map size");
        }

        _colors = std::move(colors);
        _colorIndices = std::move(colorIndices);
        _colorsDirty = _colorIndicesDirty = true;
        markModified(false);
    }

private:
    void _initialize()
    {
        _handle = ospNewGeometricModel();
        // Default white opaque matte material
        setMaterial(std::make_unique<MatteMaterial>());
    }

    Geometry<T> _geometry;

    std::vector<Vector4f> _colors;
    bool _colorsDirty {false};
    std::vector<uint8_t> _colorIndices;
    bool _colorIndicesDirty {false};

    Material::Ptr _material {nullptr};

    OSPGeometricModel _handle {nullptr};
};
}
