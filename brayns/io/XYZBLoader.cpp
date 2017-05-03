/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include <brayns/common/log.h>
#include <brayns/common/scene/Scene.h>
#include <fstream>

#include "XYZBLoader.h"

namespace brayns
{
XYZBLoader::XYZBLoader(const GeometryParameters& geometryParameters)
    : _geometryParameters(geometryParameters)
{
}

bool XYZBLoader::importFromFile(const std::string& filename, Scene& scene)
{
    BRAYNS_INFO << "Loading xyz file from " << filename << std::endl;
    std::ifstream file(filename, std::ios::in);
    if (!file.good())
    {
        BRAYNS_ERROR << "Could not open file " << filename << std::endl;
        return false;
    }

    SpheresMap& spheres = scene.getSpheres();
    bool validParsing = true;
    std::string line;

    while (validParsing && std::getline(file, line))
    {
        std::vector<float> lineData;
        std::stringstream lineStream(line);

        float value;
        while (lineStream >> value)
            lineData.push_back(value);

        switch (lineData.size())
        {
        case 3:
        {
            const Vector3f position(lineData[0], lineData[1], lineData[2]);
            BRAYNS_INFO << position << std::endl;
            spheres[0].push_back(SpherePtr(
                new Sphere(position, _geometryParameters.getRadiusMultiplier(),
                           0.f, 0.f)));
            scene.getWorldBounds().merge(position);
            break;
        }
        default:
            BRAYNS_ERROR << "Invalid line: " << line << std::endl;
            validParsing = false;
            break;
        }
    }

    file.close();
    return validParsing;
}

bool XYZBLoader::importFromBinaryFile(const std::string& filename, Scene& scene)
{
    BRAYNS_INFO << "Loading xyzb file from " << filename << std::endl;
    std::ifstream file(filename, std::ios::in | std::fstream::binary);
    if (!file.good())
    {
        BRAYNS_ERROR << "Could not open file " << filename << std::endl;
        return false;
    }

    file.seekg(0, std::ios_base::end);
    uint64_t nbPoints = file.tellg() / (3 * sizeof(double));
    file.seekg(0);

    SpheresMap& spheres = scene.getSpheres();
    Progress progress("Loading spheres...", nbPoints);
    while (!file.eof())
    {
        ++progress;

        double x, y, z;
        file.read((char*)&x, sizeof(double));
        file.read((char*)&y, sizeof(double));
        file.read((char*)&z, sizeof(double));

        BRAYNS_DEBUG << x << "," << y << "," << z << std::endl;

        const Vector3f position(x, y, z);
        spheres[0].push_back(SpherePtr(
            new Sphere(position, _geometryParameters.getRadiusMultiplier(), 0.f,
                       0.f)));
        scene.getWorldBounds().merge(position);
    }

    file.close();
    return true;
}
}
