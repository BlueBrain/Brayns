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

#ifndef MOLECULARSYSTEMREADER_H
#define MOLECULARSYSTEMREADER_H

#include <brayns/common/types.h>

#include <string>

namespace brayns
{

/**
 * @brief The ProteinPosition struct contains the ID of each protein as well as its positions
 *        in the system
 */
typedef std::map< size_t, Vector3fs > ProteinPositions;

/**
 * @brief The MolecularSystemReader class loads proteins and meshes according to parameters
 *        defined in a Molecular System Configuration file. This configuration is defined by
 *        the following entries:
 *        - ProteinFolder: Folder containing pdb files
 *        - MeshFolder: Folder containing obj files
 *        - SystemDescriptor: File containing the IDs of the proteins
 *        - ProteinPositions: File containing the position of each protein
 */
class MolecularSystemReader
{

public:

    /**
     * @brief Default constructor
     * @param geometryParameters Geometry parameters
     */
    MolecularSystemReader( const GeometryParameters& geometryParameters );

    /**
     * @brief Imports proteins and shapes from the Molecular System Configutation file, defined
     *        by the --molecular-system-config command line parameter
     * @param Scene Resulting scene
     * @return True if the system is successfully loaded, false otherwise
     */
    bool import( Scene& scene );

private:

    const GeometryParameters& _geometryParameters;

};

}

#endif // MOLECULARSYSTEMREADER_H
