/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <brayns/common/types.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/parameters/GeometryParameters.h>
#include <brayns/common/geometry/TrianglesMesh.h>

#include <string>

class aiScene;

namespace brayns
{

/** structure containing references to triangles, materials and bounding for
    for all meshes
*/
struct MeshContainer
{
    TrianglesMeshMap& triangles;
    Materials& materials;
    Boxf& bounds;
};

/** Loads meshes from files using the assimp library
 * http://assimp.sourceforge.net
 */
class MeshLoader
{
public:
    MeshLoader();

    /** Imports meshes from a given file
     *
     * @param filename name of the file containing the meshes
     * @param meshContainer structure containing references to triangles and
     *        materials imported from the file, as well as the bounding box of
     *        all meshes.
     * @param material is set to NO_MATERIAL, materials defined in the source
     *        file are used. Otherwise, the specified material is assigned to
     *        all imported meshes.
     * @param meshQuality can be MQ_FAST, MQ_QUALITY or MQ_MAX_QUALITY. Appart
     *        from MQ_FAST, normals are automatically generated is not in the
     *        file.
     * @param bounds resulting bounding box of the loaded meshes
     */
    bool importMeshFromFile(
            const std::string& filename,
            MeshContainer& meshContainer,
            MeshQuality meshQuality,
            int material);

    /** Exports meshes to a given file
     *
     * @param filename destination file name
     * @param meshContainer structure containing references to triangles and
     *        materials imported from the file, as well as the bounding box of
     *        all meshes.
     */
    bool exportMeshToFile(
            const std::string& filename,
            MeshContainer& meshContainer ) const;

private:
    void _createMaterials(
        const aiScene *scene,
        const std::string& folder,
        Materials& materials );

    std::map<size_t, size_t> _meshIndex;
};

}

#endif // MESHLOADER_H
