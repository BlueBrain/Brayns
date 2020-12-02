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
    ~PBRTModel() = default;

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

    const std::vector<std::shared_ptr<pbrt::Light>>& getModelLights()
    {
        return _modelLights;
    }

protected:
    void _commitSimulationDataImpl(const float *frameData, const size_t frameSize) final
    {
        _simulationData = std::vector<float>(frameData, frameData + frameSize);
    }

    void _commitTransferFunctionImpl(const Vector3fs&, const floats&, const Vector2d) final
    {
        // Do nothing, we simply access the model transfer function
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
    Primitives _createSpheres(const pbrt::Transform& transform);
    Primitives _createCylinders(const pbrt::Transform& transform);
    Primitives _createCones(const pbrt::Transform& transform);
    Primitives _createMeshes(const pbrt::Transform& transform);
    Primitives _createSDFGeometries(const pbrt::Transform& transform);

    void _parseMedium(pbrt::Transform* otw);

    std::vector<float> _simulationData;
    std::vector<std::shared_ptr<pbrt::Light>> _modelLights;
};
}

#endif
