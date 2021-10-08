/* Copyright (c) 2020, EPFL/Blue Brain Project
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

#include <brayns/common/log.h>

#include <brayns/engine/Model.h>

#include <pbrtv2/core/sensor.h>
#include <pbrtv2/core/shape.h>

#include <functional>
#include <unordered_map>

namespace brayns
{
class PBRTModel : public Model
{
public:
    PBRTModel(AnimationParameters& animationParameters,
              VolumeParameters& volumeParameters);
    ~PBRTModel();

    void commitGeometry() final;

    std::vector<pbrt::Reference<pbrt::Primitive>> commitToPBRT(
        const Transformation& transform,
        std::vector<pbrt::Sensor*>& sensorBuffer,
        std::vector<pbrt::VolumeRegion*>& volumeBuffer);

    void buildBoundingBox() final;

    SharedDataVolumePtr createSharedDataVolume(const Vector3ui& dimensions,
                                               const Vector3f& spacing,
                                               const DataType type) const final;

    BrickedVolumePtr createBrickedVolume(const Vector3ui& dimensions,
                                         const Vector3f& spacing,
                                         const DataType type) const final
    {
        (void)dimensions;
        (void)spacing;
        (void)type;
        throw std::runtime_error(
            "PBRTModel: createBrickedVolume() not implemented");
    }

    bool materialsDirty() const;

protected:
    // Simulations will require a custom integrator
    // Disabled by now
    void _commitSimulationDataImpl(const float* frameData,
                                   const size_t frameSize) final
    {
        (void)frameData;
        (void)frameSize;
        BRAYNS_WARN << "PBRTModel commitSimulationDataImpl: "
                    << "Simulations not supported yet for PBRT" << std::endl;
    }
    void _commitTransferFunctionImpl(const Vector3fs& colors,
                                     const floats& opacities,
                                     const Vector2d valueRange) final
    {
        (void)colors;
        (void)opacities;
        (void)valueRange;
        BRAYNS_WARN << "PBRTModel commitSimulationDataImpl: "
                    << "Simulations not supported yet for PBRT" << std::endl;
    }

private:
    // Instantiate all the scene materials
    void _commitMaterials();
    MaterialPtr createMaterialImpl(const PropertyMap& properties = {}) final;

    // Holds all the transforms of this model shapes
    std::vector<std::unique_ptr<pbrt::Transform>> _transformPool;

    // Global model transforms
    pbrt::Transform _objectToWorld;
    pbrt::Transform _worldToObject;

    // Functionality to instantiate volumes from metaobjects
    using VolumeFactory =
        std::function<pbrt::VolumeRegion*(pbrt::Transform*,
                                          const PropertyMap&)>;
    static std::unordered_map<std::string, VolumeFactory> _volumeFactories;
    std::vector<pbrt::VolumeRegion*> _createVolumes(pbrt::Transform* otw);

    // Transform Brayns objects into PBRT objects
    using Primitives = std::vector<pbrt::Reference<pbrt::Primitive>>;
    Primitives _createSpheres(pbrt::Transform* otw, pbrt::Transform* wto);
    Primitives _createCylinders(pbrt::Transform* otw, pbrt::Transform* wto);
    Primitives _createCones(pbrt::Transform* otw, pbrt::Transform* wto);
    Primitives _createMeshes(pbrt::Transform* otw, pbrt::Transform* wto);
    Primitives _createSDFGeometries(pbrt::Transform* otw, pbrt::Transform* wto);

    // Functionality to instantiate sensors from metaobjects
    std::vector<pbrt::Sensor*> _createSensors(pbrt::Transform* otw,
                                              pbrt::Transform* wto);
};
} // namespace brayns
