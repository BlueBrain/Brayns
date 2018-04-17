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

#include <brayns/common/geometry/Model.h>
#include <brayns/common/material/MaterialManager.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>
#include <brayns/parameters/GeometryParameters.h>

#include <fstream>

namespace
{
const float CALCIUM_RADIUS = 0.00194f;
}

namespace brayns
{
CADiffusionSimulationHandler::CADiffusionSimulationHandler(
    MaterialManager& materialManager, const std::string& simulationFolder)
    : _materialManager(materialManager)
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

void CADiffusionSimulationHandler::setFrame(Model& model, const size_t frame)
{
    BRAYNS_DEBUG << "Setting Calcium Positions frame to " << frame << std::endl;
    if (frame == _currentFrame)
        return;

    _currentFrame = frame;

    // Load Calcium positions
    _loadCalciumPositions(frame);
    Material material;
    material.setDiffuseColor({1.f, 1.f, 1.f});
    material.setName("Calcium");
    const size_t materialId = _materialManager.add(material);
    if (!_spheresCreated)
    {
        BRAYNS_INFO << "Creating " << _calciumPositions.size() << " CA spheres"
                    << std::endl;
        for (const auto position : _calciumPositions)
            model.addSphere(materialId, {position, CALCIUM_RADIUS});
        _spheresCreated = true;
    }
    else
    {
        uint64_t i = 0;
        for (const auto position : _calciumPositions)
        {
            model.setSphere(i, materialId, Sphere(position, CALCIUM_RADIUS));
            ++i;
        }
    }
}
}
