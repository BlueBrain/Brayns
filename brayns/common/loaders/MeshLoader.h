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
#include <brayns/common/parameters/GeometryParameters.h>

#include <string>

#define NO_MATERIAL -1

class aiScene;

namespace brayns
{

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
     * @param scale scale ratio applied to mesh geometry
     * @param triangles triangles to populate
     * @param materials materials to populate
     * @param bounds resulting bounding box of the loaded meshes
     * @param material is set to NO_MATERIAL, materials defined in the source
     *        file are used. Otherwise, the specified material is assigned to
     *        all imported meshes.
     */
    bool importMeshFromFile(
            const std::string& filename,
            float scale,
            TrianglesCollection& triangles,
            MaterialsCollection& materials,
            box3f& bounds,
            int material = NO_MATERIAL);

#ifdef __APPLE__
    /** Exports meshes to a given file
     *
     * @param filename destination file name
     * @param triangles triangles defining the mesh
     * @param materials mesh materials
     */
    bool exportMeshToFile(
            const std::string& filename,
            TrianglesCollection& triangles,
            MaterialsCollection& materials ) const;
#endif

private:

    void createMaterials_(
        const aiScene *scene,
        MaterialsCollection& materials,
        int defaultMaterial );

    std::map< size_t, size_t > meshIndex_;
};

}

#endif // MESHLOADER_H
