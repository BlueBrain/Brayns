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

#ifndef PBRTMODEL_H
#define PBRTMODEL_H

#include <brayns/common/log.h>

#include <brayns/engine/Model.h>

#include <pbrt/core/medium.h>
#include <pbrt/core/shape.h>

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

    std::vector<std::shared_ptr<pbrt::GeometricPrimitive>>
    commitToPBRT(const Transformation& transform, const std::string& renderer);

    void buildBoundingBox() final;

    SharedDataVolumePtr createSharedDataVolume(const Vector3ui &dimensions,
                                               const Vector3f &spacing,
                                               const DataType type) const final;

    BrickedVolumePtr createBrickedVolume(const Vector3ui &dimensions, const Vector3f &spacing,
                                         const DataType type) const final
    {
        (void)dimensions;
        (void)spacing;
        (void)type;
        throw std::runtime_error("PBRTModel: createBrickedVolume() not implemented");
    }

    bool materialsDirty() const;

protected:
    // Simulations will require a custom integrator
    // Disabled by now
    void _commitSimulationDataImpl(const float *frameData, const size_t frameSize) final
    {
        (void)frameData;
        (void)frameSize;
        BRAYNS_WARN << "PBRTModel commitSimulationDataImpl: "
                    << "Simulations not supported yet for PBRT" << std::endl;
    }
    void _commitTransferFunctionImpl(const Vector3fs &colors, const floats &opacities,
                                     const Vector2d valueRange) final
    {
        (void)colors;
        (void)opacities;
        (void)valueRange;
        BRAYNS_WARN << "PBRTModel commitSimulationDataImpl: "
                    << "Simulations not supported yet for PBRT" << std::endl;
    }
private:
    // Material commitment function.
    void _commitMaterials(const std::string& renderer);
    MaterialPtr createMaterialImpl(const PropertyMap &properties = {}) final;

    // Shapes transform pool
    std::vector<std::unique_ptr<pbrt::Transform>> _transformPool;

    // Call backs to handle different type of media
    using MediaFactory = std::function<pbrt::Medium*(pbrt::Transform*,
                                                     const brayns::PropertyMap&)>;
    static std::unordered_map<std::string, MediaFactory> _mediaFactories;
    // Media object
    std::shared_ptr<pbrt::Medium> _modelMedium {nullptr};

    // Model global transforms
    pbrt::Transform _objectToWorld;
    pbrt::Transform _worldToObject;

    // Auxiliary functions to translate Brayns objects into PBRT objects
    using Primitives = std::vector<std::shared_ptr<pbrt::GeometricPrimitive>>;
    Primitives _createSpheres(pbrt::Transform* otw, pbrt::Transform* wto);
    Primitives _createCylinders(pbrt::Transform* otw, pbrt::Transform* wto);
    Primitives _createCones(pbrt::Transform* otw, pbrt::Transform* wto);
    Primitives _createMeshes(pbrt::Transform* otw, pbrt::Transform* wto);
    Primitives _createSDFGeometries(pbrt::Transform* otw, pbrt::Transform* wto);

    void _parseMedium(pbrt::Transform* otw);
};
}

#endif
