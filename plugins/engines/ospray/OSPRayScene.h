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

#include <fstream>

namespace brayns
{
/**

   OSPRray specific scene

   This object is the OSPRay specific implementation of a scene

*/
class OSPRayScene : public brayns::Scene
{
public:
    OSPRayScene(Renderers renderer, ParametersManager& parametersManager);
    ~OSPRayScene();

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

    /** @copydoc Scene::reset */
    void reset() final;

    /** @copydoc Scene::saveSceneToCacheFile */
    void saveSceneToCacheFile() final;

    /** @copydoc Scene::isVolumeSupported */
    bool isVolumeSupported(const std::string& volumeFile) const final;

    OSPModel* modelImpl(const size_t timestamp);
    OSPModel* simulationModelImpl() { return &_simulationModel; };
private:
    OSPTexture2D _createTexture2D(const std::string& textureName);
    void _createModel(const size_t timestamp);
    uint32_t _getOSPDataFlags();

    uint64_t _serializeSpheres(const size_t materialId);
    uint64_t _serializeCylinders(const size_t materialId);
    uint64_t _serializeCones(const size_t materialId);
    uint64_t _buildMeshOSPGeometry(const size_t materialId);

    void _loadCacheFile();
    void _saveCacheFile();

    std::map<size_t, OSPModel> _models;
    OSPModel _simulationModel;
    std::vector<OSPMaterial> _ospMaterials;
    std::map<std::string, OSPTexture2D> _ospTextures;

    std::vector<OSPLight> _ospLights;
    OSPData _ospLightData;
    OSPData _ospMaterialData;
    OSPData _ospVolumeData;
    OSPData _ospSimulationData;
    OSPData _ospTransferFunctionDiffuseData;
    OSPData _ospTransferFunctionEmissionData;

    std::map<size_t, OSPGeometry> _ospExtendedSpheres;
    std::map<size_t, OSPData> _ospExtendedSpheresData;
    std::map<size_t, OSPGeometry> _ospExtendedCylinders;
    std::map<size_t, OSPData> _ospExtendedCylindersData;
    std::map<size_t, OSPGeometry> _ospExtendedCones;
    std::map<size_t, OSPData> _ospExtendedConesData;
    std::map<size_t, OSPGeometry> _ospMeshes;

    std::map<float, size_t> _timestamps;

    std::map<size_t, floats> _serializedSpheresData;
    std::map<size_t, floats> _serializedCylindersData;
    std::map<size_t, floats> _serializedConesData;
    std::map<size_t, size_t> _serializedSpheresDataSize;
    std::map<size_t, size_t> _serializedCylindersDataSize;
    std::map<size_t, size_t> _serializedConesDataSize;

    std::map<size_t, std::map<size_t, size_t>> _timestampSpheresIndices;
    std::map<size_t, std::map<size_t, size_t>> _timestampCylindersIndices;
    std::map<size_t, std::map<size_t, size_t>> _timestampConesIndices;

    float _currentTimestamp;
};
}
#endif // OSPRAYSCENE_H
