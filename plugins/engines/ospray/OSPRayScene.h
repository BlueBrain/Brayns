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

#ifndef OSPRAYSCENE_H
#define OSPRAYSCENE_H

#include <brayns/common/scene/Scene.h>
#include <brayns/common/types.h>

#include <ospray_cpp/Data.h>
#include <ospray_cpp/Light.h>
#include <ospray_cpp/Model.h>
#include <ospray_cpp/Texture2D.h>

#include "OSPRayMaterialManager.h"

namespace brayns
{
/**

   OSPRray specific scene

   This object is the OSPRay specific implementation of a scene

*/
class OSPRayScene : public brayns::Scene
{
public:
    OSPRayScene(Renderers renderer, ParametersManager& parametersManager,
                MaterialManager& materialManager,
                const size_t memoryManagementFlags);
    ~OSPRayScene();

    /** @copydoc Scene::commit */
    void commit() final;

    /** @copydoc Scene::commitLights */
    void commitLights() final;

    /** @copydoc Scene::commitMaterials */
    void commitMaterials() final;

    /** @copydoc Scene::commitSimulationData */
    void commitSimulationData() final;

    /** @copydoc Scene::commitVolumeData */
    void commitVolumeData() final;

    /** @copydoc Scene::commitTransferFunctionData */
    void commitTransferFunctionData() final;

    /** @copydoc Scene::unload */
    void unload() final;

    /** @copydoc Scene::isVolumeSupported */
    bool isVolumeSupported(const std::string& volumeFile) const final;

    /** @copydoc Scene::addGeometryGroup */
    GeometryGroupPtr addGeometryGroup(const std::string& name,
                                      const std::string& uri) final;

    /** @copydoc Scene::removeGeometryGroup */
    void removeGeometryGroup(const size_t index) final;

    OSPModel getModel() { return _rootModel; }
    OSPModel simulationModelImpl() { return _rootSimulationModel; }
private:
    OSPTexture2D _createTexture2D(const std::string& textureName);

    OSPModel _getActiveModel();

    OSPModel _rootModel{nullptr};
    std::vector<GeometryGroupAttributes> _ospGeometryGroups;
    OSPModel _rootSimulationModel{nullptr};

    std::vector<OSPLight> _ospLights;
    OSPData _ospLightData{nullptr};
    OSPData _ospVolumeData{nullptr};
    uint64_t _ospVolumeDataSize{0};
    OSPData _ospSimulationData{nullptr};
    OSPData _ospTransferFunctionEmissionData{nullptr};
    OSPData _ospTransferFunctionDiffuseData{nullptr};

    size_t _memoryManagementFlags{0};
};
}
#endif // OSPRAYSCENE_H
