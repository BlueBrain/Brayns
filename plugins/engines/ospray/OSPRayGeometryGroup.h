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

#include "OSPRayMaterialManager.h"

#include <brayns/common/geometry/GeometryGroup.h>
#include <brayns/parameters/ParametersManager.h>

#include <ospray_cpp/Data.h>

namespace brayns
{
class OSPRayGeometryGroup : public GeometryGroup
{
public:
    OSPRayGeometryGroup(MaterialManagerPtr materialManager);
    ~OSPRayGeometryGroup() final;

    void setMemoryFlags(const size_t memoryManagementFlags);

    void unload() final;
    void commitMaterials(const uint32_t flags);
    uint64_t commit();
    OSPModel getModel() { return _model; }
    OSPGeometry getInstance(const Vector3f& translation,
                            const Vector3f& rotation, const Vector3f& scale);
    OSPGeometry getSimulationModelInstance(const Vector3f& translation,
                                           const Vector3f& rotation,
                                           const Vector3f& scale);

private:
    uint64_t _commitSpheres(const size_t materialId);
    uint64_t _commitCylinders(const size_t materialId);
    uint64_t _commitCones(const size_t materialId);
    uint64_t _commitMeshes(const size_t materialId);

    OSPModel _model{nullptr};
    OSPModel _simulationModel{nullptr};
    OSPGeometry _instance{nullptr};
    OSPGeometry _simulationModelInstance{nullptr};

    std::map<size_t, OSPGeometry> _ospExtendedSpheres;
    std::map<size_t, OSPData> _ospExtendedSpheresData;
    std::map<size_t, OSPGeometry> _ospExtendedCylinders;
    std::map<size_t, OSPData> _ospExtendedCylindersData;
    std::map<size_t, OSPGeometry> _ospExtendedCones;
    std::map<size_t, OSPData> _ospExtendedConesData;
    std::map<size_t, OSPGeometry> _ospMeshes;

    size_t _memoryManagementFlags{OSP_DATA_SHARED_BUFFER};
};
}
#endif // OSPRAYGEOMETRYGROUP_H
