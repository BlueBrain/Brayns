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

#ifndef SceneLoader_H
#define SceneLoader_H

#include <brayns/common/types.h>
#include <brayns/io/MeshLoader.h>

namespace brayns
{
/**
 * Loads meshes according to given positions in space
 */
class SceneLoader
{
public:
    SceneLoader(const GeometryParameters& geometryParameters);

    /** Imports a file containing the positions and the filename of a list
     * of meshes. All loaded meshes are positioned at the corresponding
     * coordinates. The file is ASCII and has the following format for every
     * line:
     *   x,y,z,filename
     *
     * @param filename name of the file containing the positions and meshes
     *        filenames
     * @param Scene holding the meshes
     * @param meshLoader Loader used to load a meshes
     * @return true if the file was successfully imported. False otherwise.
     */
    bool importFromFile(const std::string& filename, Scene& scene,
                        MeshLoader& meshLoader);

private:
    struct Node
    {
        Vector3f position;
        uint16_t materialId;
        std::string filename;
    };
    typedef std::vector<Node> Nodes;

    bool _parsePositions(const std::string& filename);
    void _importMeshes(Scene& scene, MeshLoader& meshLoader);

    const GeometryParameters& _geometryParameters;

    Nodes _nodes;
};
}

#endif // SceneLoader_H
