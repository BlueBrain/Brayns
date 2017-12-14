/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef OPTIXSCENE_H
#define OPTIXSCENE_H

#include <brayns/common/scene/Scene.h>
#include <brayns/common/types.h>

#include <optixu/optixpp_namespace.h>

namespace brayns
{
struct BasicLight
{
    optix::float3 pos;
    optix::float3 color;
    int casts_shadow;
    int padding; // make this structure 32 bytes -- powers of two are your
                 // friend!
};

/**

   OptiX specific scene

   This object is the OptiX specific implementation of a scene

*/
class OptiXScene : public brayns::Scene
{
public:
    OptiXScene(Renderers renderer, ParametersManager& parametersManager,
               optix::Context& context);

    ~OptiXScene() = default;

    /** @copydoc Scene::commit */
    void commit() final;

    /** @copydoc Scene::buildGeometry */
    void buildGeometry() final;

    /** @copydoc Scene::serializeGeometry */
    uint64_t serializeGeometry() final;

    /** @copydoc Scene::commitLights */
    void commitLights() final;

    /** @copydoc Scene::commitMaterials */
    void commitMaterials(const bool updateOnly = false) final;

    /** @copydoc Scene::commitSimulationData */
    void commitSimulationData() final;

    /** @copydoc Scene::commitVolumeData */
    void commitVolumeData() final;

    /** @copydoc Scene::commitTransferFunctionData */
    void commitTransferFunctionData() final;

    /** @copydoc Scene::unload */
    void unload() final;

    /** @copydoc Scene::loadFromCacheFile */
    void loadFromCacheFile() final;

    /** @copydoc Scene::saveToCacheFile */
    void saveToCacheFile() final;

    /** @copydoc Scene::isVolumeSupported */
    bool isVolumeSupported(const std::string& volumeFile) const final;

    /** @internal */
    void reset() final;

private:
    void _processVolumeAABBGeometry();
    bool _createTexture2D(const std::string& textureName);

    uint64_t _getBvhSize(const uint64_t nbElements) const;

    uint64_t _serializeSpheres(const size_t materialId);
    uint64_t _serializeCylinders(const size_t materialId);
    uint64_t _serializeCones(const size_t materialId);
    uint64_t _processMeshes();

    optix::Context& _context;
    optix::GeometryGroup _geometryGroup;
    std::vector<optix::GeometryInstance> _geometryInstances;
    std::vector<optix::Material> _optixMaterials;
    optix::Buffer _lightBuffer;
    std::vector<BasicLight> _optixLights;
    std::string _accelerationStructure;

    // Material Lookup tables
    optix::Buffer _colorMapBuffer;
    optix::Buffer _emissionIntensityMapBuffer;

    // Spheres
    std::map<size_t, optix::Buffer> _spheresBuffers;
    std::map<size_t, optix::Geometry> _optixSpheres;

    // Cylinders
    std::map<size_t, optix::Buffer> _cylindersBuffers;
    std::map<size_t, optix::Geometry> _optixCylinders;

    // Cones
    std::map<size_t, optix::Buffer> _conesBuffers;
    std::map<size_t, optix::Geometry> _optixCones;

    // Triangle meshes
    optix::Geometry _mesh;
    optix::Buffer _verticesBuffer;
    optix::Buffer _indicesBuffer;
    optix::Buffer _normalsBuffer;
    optix::Buffer _textureCoordsBuffer;
    optix::Buffer _materialsBuffer;

    // Volume
    optix::Buffer _volumeBuffer;

    // Textures
    std::map<std::string, optix::Buffer> _optixTextures;
    std::map<std::string, optix::TextureSampler> _optixTextureSamplers;

    // Programs
    optix::Program _phong_ch;
    optix::Program _phong_ch_textured;
    optix::Program _phong_ah;

    optix::Program _spheresBoundsProgram;
    optix::Program _spheresIntersectProgram;

    optix::Program _cylindersBoundsProgram;
    optix::Program _cylindersIntersectProgram;

    optix::Program _conesBoundsProgram;
    optix::Program _conesIntersectProgram;

    optix::Program _meshBoundsProgram;
    optix::Program _meshIntersectProgram;
};
}
#endif // OPTIXSCENE_H
