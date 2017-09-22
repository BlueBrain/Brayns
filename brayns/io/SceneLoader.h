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
    SceneLoader(const ApplicationParameters& applicationParameters,
                const GeometryParameters& geometryParameters);

    /** Imports a file containing the positions and the filename of a list
     * of meshes. All loaded meshes are positioned at the corresponding
     * coordinates. The file is a space separate ASCII and has the following
     * format for every line:
     *
     *   x y z type filename
     *
     * type is the kind of geometry that will be loaded (point, morphology or
     * mesh)
     *
     * @param filename name of the file containing the positions and geometry
     *        filenames
     * @param Scene holding the geometry
     * @param meshLoader Loader used to load meshes
     * @return true if the file was successfully imported. False otherwise.
     */
    bool importFromFile(const std::string& filename, Scene& scene,
                        MeshLoader& meshLoader);

private:
    enum class FileType
    {
        point = 0,
        morphology = 1,
        mesh = 2
    };

    struct Node
    {
        Vector3f position;
        uint16_t materialId;
        FileType fileType;
        std::string filename;
    };
    typedef std::vector<Node> Nodes;

    bool _parsePositions(const std::string& filename);
#ifdef BRAYNS_USE_BRION
    void _importMorphology(Scene& scene, const Node& node,
                           const Matrix4f& transformation);
#endif
#ifdef BRAYNS_USE_ASSIMP
    void _importMesh(Scene& scene, MeshLoader& loader, const Node& node,
                     const Matrix4f& transformation);
#endif
    bool _processNodes(Scene& scene, MeshLoader& loader);

    const ApplicationParameters& _applicationParameters;
    const GeometryParameters& _geometryParameters;

    Nodes _nodes;
};
}

#endif // SceneLoader_H
