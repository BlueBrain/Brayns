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

#ifndef PROTEINLOADER_H
#define PROTEINLOADER_H

#include <brayns/common/types.h>
#include <brayns/common/parameters/GeometryParameters.h>
#include <string>

namespace brayns
{

enum MaterialScheme
{
    ms_atoms,
    ms_chains,
    ms_residues,
    ms_backbone
};

/** Loads protein from PDB files
 * http://www.rcsb.org
 */
class ProteinLoader
{
public:
    ProteinLoader( const GeometryParameters& geometryParameters );

    /** Imports atoms from a given PDB file into spheres
     *
     * @param folder folder containing PDB files
     * @param cellFile name of file containing cell definiton (number of
     *        instances of each protein as well as its unique id in the cell).
     * @param positionFile name of file containing protein positions
     * @param materialScheme scheme used for materials (0: atoms, 1: chains,
     *        2:residues)
     * @param materials materials to assign to atoms
     * @param geometries resulting geometries
     * @param bounds resulting bounding box
     */
    void importPDBFolder(
        const std::string& folder,
        const std::string& cellFile,
        const std::string& positionsFile,
        const MaterialScheme materialScheme,
        const MaterialsCollection& materials,
        Geometries& geometries,
        box3f& bounds );

    /** Imports atoms from a given PDB file into spheres
     *
     * @param filename filename of the PDB file
     * @param position translates the protein to the given position
     * @param materialScheme scheme used for materials
     * @param materials materials to assign to atoms
     * @param geometries resulting geometries
     * @param bounds resulting bounding of the scene
     * @return bounding box of the protein
     */
    box3f importPDBFile(
        const std::string &filename,
        const ospray::vec3f& position,
        const MaterialScheme materialScheme,
        int material,
        const MaterialsCollection& materials,
        Geometries& geometries,
        box3f& bounds );

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
        float& b);

private:

    GeometryParameters geometryParameters_;

};

}

#endif // PROTEINLOADER_H
