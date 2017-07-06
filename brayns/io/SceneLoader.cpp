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

#include "SceneLoader.h"

#include <brayns/common/log.h>
#include <brayns/common/scene/Scene.h>
#include <fstream>

namespace brayns
{
SceneLoader::SceneLoader(const GeometryParameters& geometryParameters)
    : _geometryParameters(geometryParameters)
{
}

bool SceneLoader::_parsePositions(const std::string& filename)
{
    _nodes.clear();

    BRAYNS_INFO << "Loading nodes from " << filename << std::endl;
    std::ifstream file(filename, std::ios::in);
    if (!file.good())
    {
        BRAYNS_ERROR << "Could not open file " << filename << std::endl;
        return false;
    }
    bool validParsing = true;
    std::string line;
    while (validParsing && std::getline(file, line))
    {
        std::vector<std::string> lineData;
        std::stringstream lineStream(line);

        std::string value;
        while (lineStream >> value)
            lineData.push_back(value);

        switch (lineData.size())
        {
        case 5:
        {
            Node node;
            node.position = Vector3f(boost::lexical_cast<float>(lineData[0]),
                                     boost::lexical_cast<float>(lineData[1]),
                                     boost::lexical_cast<float>(lineData[2]));
            node.materialId = boost::lexical_cast<uint16_t>(lineData[3]);
            node.filename = lineData[4];
            _nodes.push_back(node);
            break;
        }
        default:
            BRAYNS_ERROR << "Invalid line: " << line << std::endl;
            validParsing = false;
            break;
        }
    }
    file.close();
    BRAYNS_INFO << _nodes.size() << " nodes loaded" << std::endl;
    return true;
}

void SceneLoader::_importMeshes(Scene& scene, MeshLoader& meshLoader)
{
    // Load mesh at specified positions
    uint32_t count = 0;
    Progress progress("Loading scene...", _nodes.size());
    for (const auto& node : _nodes)
    {
        ++progress;
        Matrix4f matrix;
        matrix.setTranslation(node.position);
        if (!meshLoader.importMeshFromFile(
                node.filename, scene, _geometryParameters.getGeometryQuality(),
                matrix, NB_SYSTEM_MATERIALS + node.materialId))
        {
            BRAYNS_DEBUG << "Failed to load " << node.filename << std::endl;
        }
        ++count;
    }
}

bool SceneLoader::importFromFile(const std::string& filename, Scene& scene,
                                 MeshLoader& meshLoader)
{
    if (_parsePositions(filename))
        _importMeshes(scene, meshLoader);
    else
        return false;

    return true;
}
}
