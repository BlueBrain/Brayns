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

#include <brayns/engine/Model.h>

#include <ospray.h>

namespace brayns
{
class OSPRayModel : public Model
{
public:
    OSPRayModel(AnimationParameters& animationParameters,
                VolumeParameters& volumeParameters);
    ~OSPRayModel() final;

    void setMemoryFlags(const size_t memoryManagementFlags);

    void commitGeometry() final;
    void commitMaterials(const std::string& renderer);

    OSPModel getPrimaryModel() const { return _primaryModel; }
    OSPModel getSecondaryModel() const { return _secondaryModel; }
    OSPModel getBoundingBoxModel() const { return _boundingBoxModel; }
    SharedDataVolumePtr createSharedDataVolume(const Vector3ui& dimensions,
                                               const Vector3f& spacing,
                                               const DataType type) const final;
    BrickedVolumePtr createBrickedVolume(const Vector3ui& dimensions,
                                         const Vector3f& spacing,
                                         const DataType type) const final;

    void buildBoundingBox() final;

    OSPData simulationData() const { return _ospSimulationData; }
    OSPTransferFunction transferFunction() const
    {
        return _ospTransferFunction;
    }

protected:
    void _commitTransferFunctionImpl(const Vector3fs& colors,
                                     const floats& opacities,
                                     const Vector2d valueRange) final;
    void _commitSimulationDataImpl(const float* frameData,
                                   const size_t frameSize) final;

private:
    using GeometryMap = std::map<size_t, OSPGeometry>;

    OSPGeometry& _createGeometry(GeometryMap& map, size_t materialID,
                                 const char* name);
    void _commitSpheres(const size_t materialId);
    void _commitCylinders(const size_t materialId);
    void _commitCones(const size_t materialId);
    void _commitMeshes(const size_t materialId);
    void _commitStreamlines(const size_t materialId);
    void _commitSDFGeometries();
    void _addGeometryToModel(const OSPGeometry geometry,
                             const size_t materialId);
    void _setBVHFlags();

    // Models
    OSPModel _primaryModel{nullptr};
    OSPModel _secondaryModel{nullptr};
    OSPModel _boundingBoxModel{nullptr};

    // Bounding box
    size_t _boudingBoxMaterialId{0};

    // Simulation model
    OSPData _ospSimulationData{nullptr};

    OSPTransferFunction _ospTransferFunction{nullptr};

    // OSPRay data
    std::map<size_t, OSPGeometry> _ospSpheres;
    std::map<size_t, OSPGeometry> _ospCylinders;
    std::map<size_t, OSPGeometry> _ospCones;
    std::map<size_t, OSPGeometry> _ospMeshes;
    std::map<size_t, OSPGeometry> _ospStreamlines;
    std::map<size_t, OSPGeometry> _ospSDFGeometries;

    size_t _memoryManagementFlags{OSP_DATA_SHARED_BUFFER};

    std::string _renderer;

    MaterialPtr createMaterialImpl(const PropertyMap& properties = {}) final;
};
}
