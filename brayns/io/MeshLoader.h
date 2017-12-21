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

#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <brayns/common/geometry/TrianglesMesh.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/types.h>
#include <brayns/parameters/GeometryParameters.h>

#include <string>

class aiScene;

namespace brayns
{
/** Loads meshes from files using the assimp library
 * http://assimp.sourceforge.net
 */
class MeshLoader
{
public:
    MeshLoader(const GeometryParameters& geometryParameters);

    /** Imports meshes from a given file
     *
     * @param filename name of the file containing the meshes
     * @param scene Scene holding the meshes
     * @param transformation Position, orientation and scale to apply to the
     *        mesh
     * @param defaultMaterial Default material for the whole mesh. If set to
     *        NO_MATERIAL, materials from the mesh file are used. Otherwise,
     *        all meshes are forced to that specific material.
     * @return true if the file was successfully imported. False otherwise.
     */
    bool importMeshFromFile(const std::string& filename, Scene& scene,
                            const Matrix4f& transformation,
                            const size_t defaultMaterial);

    /** Exports meshes to a given file
     *
     * @param filename destination file name
     * @param scene Scene holding the meshes
     */
    bool exportMeshToFile(const std::string& filename, Scene& scene) const;

    /**
     * @brief Clear all internal buffers
     */
    void clear();

    /**
     * @brief getMeshFilenameFromGID Returns the name of the mesh file according
     * to the --circuit-mesh-folder, --circuit-mesh-filename-pattern command
     * line arguments and a GID
     * @param gid GID of the cell
     * @return A string with the full path of the mesh file
     */
    std::string getMeshFilenameFromGID(const uint64_t gid);

private:
#if (BRAYNS_USE_ASSIMP)
    void _createMaterials(Scene& scene, const aiScene* aiScene,
                          const std::string& folder);
#endif

    size_t _getMaterialId(const size_t materialId,
                          const size_t defaultMaterial = NO_MATERIAL);
    std::map<size_t, size_t> _meshIndex;
    const GeometryParameters& _geometryParameters;
    size_t _materialOffset;
};
}

#endif // MESHLOADER_H
