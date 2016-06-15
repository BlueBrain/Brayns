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

#ifndef PROTEINLOADER_H
#define PROTEINLOADER_H

#include <brayns/common/types.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/geometry/Primitive.h>
#include <brayns/parameters/GeometryParameters.h>
#include <string>

namespace brayns
{

/** Loads protein from PDB files
 * http://www.rcsb.org
 */
class ProteinLoader
{
public:
    ProteinLoader( const GeometryParameters& geometryParameters );

    /** Imports atoms from a given PDB file
     *
     * @return bounding box of the protein
     */
    bool importPDBFile(
        const std::string &filename,
        const Vector3f& position,
        int material,
        Scene& scene);

    /** Returns the RGB composants for a given atom index, and according to the
     * JMol scheme
     *
     * @param index index of the material
     * @param r red composant
     * @param g green composant
     * @param b blue composant
     */
    void getMaterialKd(
        size_t index,
        float& r,
        float& g,
        float& b );

private:

    GeometryParameters _geometryParameters;

};

}

#endif // PROTEINLOADER_H
