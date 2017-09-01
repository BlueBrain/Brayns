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
#include <brayns/io/MorphologyLoader.h>
#include <fstream>

#ifdef BRAYNS_USE_BRION
#include <servus/uri.h>
#endif

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
        case 6:
        {
            Node node;
            node.position = Vector3f(boost::lexical_cast<float>(lineData[0]),
                                     boost::lexical_cast<float>(lineData[1]),
                                     boost::lexical_cast<float>(lineData[2]));
            node.materialId = boost::lexical_cast<uint16_t>(lineData[3]);
            node.fileType = static_cast<FileType>(
                boost::lexical_cast<uint16_t>(lineData[4]));
            if (lineData.size() == 6)
                node.filename = lineData[5];
            _nodes.push_back(node);
            break;
        }
        default:
            BRAYNS_ERROR << "Invalid line, 5 to 6 values were expected, only "
                         << lineData.size() << " were found" << std::endl;
            BRAYNS_ERROR << line << std::endl;
            validParsing = false;
            break;
        }
    }
    file.close();
    BRAYNS_INFO << _nodes.size() << " nodes loaded" << std::endl;
    return true;
}

#ifdef BRAYNS_USE_BRION
void SceneLoader::_importMorphology(Scene& scene, const Node& node,
                                    const Matrix4f& transformation)
{
    MorphologyLoader morphologyLoader(_geometryParameters, scene);
    const servus::URI uri(node.filename);
    if (!morphologyLoader.importMorphology(uri, 0, NB_SYSTEM_MATERIALS +
                                                       node.materialId,
                                           transformation))
        BRAYNS_ERROR << "Failed to load " << node.filename << std::endl;
}
#endif

#ifdef BRAYNS_USE_ASSIMP
void SceneLoader::_importMesh(Scene& scene, MeshLoader& loader,
                              const Node& node, const Matrix4f& transformation)
{
    if (!loader.importMeshFromFile(node.filename, scene,
                                   _geometryParameters.getGeometryQuality(),
                                   transformation,
                                   NB_SYSTEM_MATERIALS + node.materialId))
        BRAYNS_ERROR << "Failed to load " << node.filename << std::endl;
}
#endif

bool SceneLoader::_processNodes(Scene& scene,
                                MeshLoader& meshLoader BRAYNS_UNUSED)
{
    Progress progress("Loading scene...", _nodes.size());
    for (const auto& node : _nodes)
    {
        ++progress;
        Matrix4f transformation;
        transformation.setTranslation(node.position);
        switch (node.fileType)
        {
        case FileType::point:
            scene.getSpheres()[node.materialId].push_back(SpherePtr(
                new Sphere(node.position,
                           _geometryParameters.getRadiusMultiplier())));
            scene.getWorldBounds().merge(node.position);
            break;
        case FileType::morphology:
#ifdef BRAYNS_USE_BRION
            _importMorphology(scene, node, transformation);
#endif
            break;
        case FileType::mesh:
#ifdef BRAYNS_USE_ASSIMP
            _importMesh(scene, meshLoader, node, transformation);
#endif
            break;
        default:
            BRAYNS_ERROR << "Unknown file type: "
                         << static_cast<size_t>(node.fileType) << std::endl;
            return false;
        }
    }
    return true;
}

bool SceneLoader::importFromFile(const std::string& filename, Scene& scene,
                                 MeshLoader& meshLoader)
{
    if (_parsePositions(filename))
        return _processNodes(scene, meshLoader);
    return false;
}
}
