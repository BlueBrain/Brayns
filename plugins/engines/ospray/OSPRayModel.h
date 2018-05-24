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

#ifndef OSPRAYGEOMETRYGROUP_H
#define OSPRAYGEOMETRYGROUP_H

#include <brayns/common/scene/Model.h>
#include <brayns/parameters/ParametersManager.h>

#include <ospray.h>

namespace brayns
{
class OSPRayModel : public Model
{
public:
    OSPRayModel() = default;
    ~OSPRayModel() final;

    void setMemoryFlags(const size_t memoryManagementFlags);

    void commit() final;

    OSPModel getModel() const { return _model; }
    OSPModel getBoundingBoxModel() const { return _boundingBoxModel; }
    OSPModel getSimulationModel() const { return _simulationModel; }
    MaterialPtr createMaterial(const size_t materialId,
                               const std::string& name) final;

    void buildBoundingBox() final;

private:
    void _commitSpheres(const size_t materialId);
    void _commitCylinders(const size_t materialId);
    void _commitCones(const size_t materialId);
    void _commitMeshes(const size_t materialId);
    void _commitSDFGeometries();

    OSPModel _model{nullptr};

    // Bounding box
    size_t _boudingBoxMaterialId{0};
    OSPModel _boundingBoxModel{nullptr};

    // Simulation model
    OSPModel _simulationModel{nullptr};

    // OSPRay data
    std::map<size_t, OSPGeometry> _ospExtendedSpheres;
    std::map<size_t, OSPData> _ospExtendedSpheresData;
    std::map<size_t, OSPGeometry> _ospExtendedCylinders;
    std::map<size_t, OSPData> _ospExtendedCylindersData;
    std::map<size_t, OSPGeometry> _ospExtendedCones;
    std::map<size_t, OSPData> _ospExtendedConesData;
    std::map<size_t, OSPGeometry> _ospMeshes;
    std::map<size_t, OSPGeometry> _ospSDFGeometryRefs;
    std::map<size_t, OSPData> _ospSDFGeometryRefsData;
    OSPData _ospSDFGeometryData = nullptr;
    OSPData _ospSDFNeighboursData = nullptr;

    size_t _memoryManagementFlags{OSP_DATA_SHARED_BUFFER};
};
}
#endif // OSPRAYGEOMETRYGROUP_H
