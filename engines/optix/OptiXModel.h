/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <brayns/engineapi/Model.h>

#include <optixu/optixpp_namespace.h>

#include <map>

namespace brayns
{
class OptiXModel : public Model
{
public:
    OptiXModel(AnimationParameters& animationParameters,
               VolumeParameters& volumeParameters);

    /** @copydoc Model::commit */
    void commitGeometry() final;

    /** @copydoc Model::buildBoundingBox */
    void buildBoundingBox() final;

    /** @copydoc Model::createMaterialImpl */
    virtual MaterialPtr createMaterialImpl(
        const PropertyMap& properties = {}) final;

    /** @copydoc Model::createSharedDataVolume */
    virtual SharedDataVolumePtr createSharedDataVolume(
        const Vector3ui& dimensions, const Vector3f& spacing,
        const DataType type) const final;

    /** @copydoc Model::createBrickedVolume */
    virtual BrickedVolumePtr createBrickedVolume(
        const Vector3ui& dimensions, const Vector3f& spacing,
        const DataType type) const final;

    ::optix::GeometryGroup getGeometryGroup() const { return _geometryGroup; }
    ::optix::GeometryGroup getBoundingBoxGroup() const
    {
        return _boundingBoxGroup;
    }

protected:
    void _commitTransferFunctionImpl(const Vector3fs& colors,
                                     const floats& opacities,
                                     const Vector2d valueRange) final;
    void _commitSimulationDataImpl(const float* frameData,
                                   const size_t frameSize) final;

private:
    void _commitSpheres(const size_t materialId);
    void _commitCylinders(const size_t materialId);
    void _commitCones(const size_t materialId);
    void _commitMeshes(const size_t materialId);
    void _commitMaterials();
    bool _commitSimulationData();
    bool _commitTransferFunction();

    ::optix::GeometryGroup _geometryGroup{nullptr};
    ::optix::GeometryGroup _boundingBoxGroup{nullptr};

    // Material Lookup tables
    ::optix::Buffer _colorMapBuffer{nullptr};
    ::optix::Buffer _emissionIntensityMapBuffer{nullptr};

    // Spheres
    std::map<size_t, ::optix::Buffer> _spheresBuffers;
    std::map<size_t, ::optix::Geometry> _optixSpheres;

    // Cylinders
    std::map<size_t, optix::Buffer> _cylindersBuffers;
    std::map<size_t, optix::Geometry> _optixCylinders;

    // Cones
    std::map<size_t, optix::Buffer> _conesBuffers;
    std::map<size_t, optix::Geometry> _optixCones;

    // Meshes
    struct TriangleMesh
    {
        optix::Buffer vertices_buffer;
        optix::Buffer normal_buffer;
        optix::Buffer texcoord_buffer;
        optix::Buffer indices_buffer;
    };

    std::map<size_t, TriangleMesh> _meshesBuffers;
    std::map<size_t, optix::Geometry> _optixMeshes;

    // Volume
    ::optix::Buffer _volumeBuffer{nullptr};

    // Materials and textures
    std::map<std::string, optix::Buffer> _optixTextures;
    std::map<std::string, optix::TextureSampler> _optixTextureSamplers;

    // Transfer function
    struct
    {
        optix::Buffer colors{nullptr};
        optix::Buffer opacities{nullptr};
    } _optixTransferFunction;

    optix::Buffer _simulationData{nullptr};

    bool _boundingBoxBuilt = false;
};
} // namespace brayns
