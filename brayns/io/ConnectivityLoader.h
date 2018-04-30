/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#ifndef CONNECTIVITY_LOADER_H
#define CONNECTIVITY_LOADER_H

#include <brayns/common/loader/Loader.h>
#include <brayns/common/types.h>
#include <brayns/parameters/GeometryParameters.h>

#include <string>

namespace brayns
{
/** Loads connectivity matrix and morphologies from H5, and Circuit Config file
 */
class ConnectivityLoader : public Loader
{
public:
    ConnectivityLoader(const GeometryParameters& geometryParameters);

    void importFromBlob(Blob&&, Scene&, const Matrix4f&, const size_t) final
    {
        throw std::runtime_error("Unsupported");
    }

    void importFromFile(const std::string&, Scene&, const Matrix4f&,
                        const size_t) final
    {
        throw std::runtime_error("Unsupported");
    }

    /**
     * @brief importFromFile loads cells from circuit (--circuit-config command
     * line argument) and creates connections according to the --connectivity-*
     * command line arguments.
     * @param scene Scene to be populated
     * @param meshLoader Loader used to load meshes
     * @return True if the loading was successful, false otherwise
     */
    bool importFromFile(Scene& scene, MeshLoader& meshLoader);

private:
    bool _importMatrix();
    bool _importMesh(const uint64_t gid, const Matrix4f& transformation,
                     const size_t materialId, Scene& scene,
                     MeshLoader& meshLoader);

    std::map<uint64_t, uint64_ts> _emitors;
    std::map<uint64_t, size_t> _receptors;
    const GeometryParameters& _geometryParameters;
};
}
#endif // CONNECTIVITY_LOADER_H
