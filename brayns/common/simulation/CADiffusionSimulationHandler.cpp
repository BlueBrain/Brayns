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

#include <brayns/common/geometry/Sphere.h>
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
    const std::string& simulationFolder)
    : _currentFrame(std::numeric_limits<size_t>::max())
    , _spheresCreated(false)
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

    // Load Calcium positions
    _loadCalciumPositions(frame);
    if (!_spheresCreated)
    {
        BRAYNS_INFO << "Creating " << _calciumPositions.size() << " CA spheres"
                    << std::endl;
        for (const auto position : _calciumPositions)
        {
            SpherePtr sphere(new Sphere(position, CALCIUM_RADIUS, 0.f, 0.f));
            scene.getSpheres()[MATERIAL_CA_SIMULATION].push_back(sphere);
            scene.getWorldBounds().merge(position);
        }
        _spheresCreated = true;
    }
    else
    {
        uint64_t i = 0;
        for (const auto position : _calciumPositions)
        {
            auto& spheres = scene.getSpheres()[MATERIAL_CA_SIMULATION];
            if (i < spheres.size())
                spheres[i]->setCenter(position);
            else
                BRAYNS_WARN << "Invalid number of positions in "
                            << _simulationFiles[frame] << std::endl;
            ++i;
        }
    }
}
}
