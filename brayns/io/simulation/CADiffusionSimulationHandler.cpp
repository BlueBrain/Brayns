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

#include "CADiffusionSimulationHandler.h"

#include <brayns/common/Transformation.h>
#include <brayns/common/utils/utils.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/GeometryParameters.h>

#include <fstream>

namespace
{
const float CALCIUM_RADIUS = 0.00194f;
const size_t CALCIUM_MATERIAL_ID = 0;
}

namespace brayns
{
CADiffusionSimulationHandler::CADiffusionSimulationHandler(
    const std::string& simulationFolder)
{
    BRAYNS_DEBUG << "Loading Calcium simulation from " << simulationFolder
                 << std::endl;
    const strings filters = {".dat"};
    strings files = parseFolder(simulationFolder, filters);
    for (size_t i = 0; i < files.size(); ++i)
    {
        BRAYNS_DEBUG << "CA diffusion: " << files[i] << std::endl;
        _simulationFiles[i] = files[i];
    }
}

bool CADiffusionSimulationHandler::_loadCalciumPositions(const size_t frame)
{
    if (_simulationFiles.find(frame) == _simulationFiles.end())
    {
        BRAYNS_ERROR << "No simulation file for frame " << frame << std::endl;
        return false;
    }

    BRAYNS_DEBUG << "Loading Calcium positions for frame " << frame
                 << ", filename: " << _simulationFiles[frame] << std::endl;

    // Load Calcium positions
    std::ifstream filePositions(_simulationFiles[frame], std::ios::in);
    if (!filePositions.good())
    {
        BRAYNS_ERROR << "Could not open file " << _simulationFiles[frame]
                     << std::endl;
        return false;
    }

    _calciumPositions.clear();
    std::string line;
    while (filePositions.good() && std::getline(filePositions, line))
    {
        std::stringstream lineStream(line);
        size_t id;
        Vector3f position;
        lineStream >> id >> position.x() >> position.y() >> position.z();

        _calciumPositions.push_back(position);
    }
    filePositions.close();
    BRAYNS_DEBUG << _calciumPositions.size() << " Calcium positions loaded"
                 << std::endl;
    return true;
}

void CADiffusionSimulationHandler::setFrame(Scene& scene, const size_t frame)
{
    BRAYNS_DEBUG << "Setting Calcium Positions frame to " << frame << std::endl;
    if (frame == _currentFrame)
        return;

    _currentFrame = frame;

    const std::string modelName = "CAFrame";

    scene.removeModel(_modelID);

    auto model = scene.createModel();

    // Load Calcium positions
    _loadCalciumPositions(frame);
    auto material = model->createMaterial(CALCIUM_MATERIAL_ID, "Calcium");
    material->setDiffuseColor({1.f, 1.f, 1.f});
    BRAYNS_INFO << "Creating " << _calciumPositions.size() << " CA spheres"
                << std::endl;
    for (const auto position : _calciumPositions)
        model->addSphere(CALCIUM_MATERIAL_ID, {position, CALCIUM_RADIUS});
    _modelID = scene.addModel(
        std::make_shared<ModelDescriptor>(std::move(model), modelName));
    scene.markModified();
}
}
