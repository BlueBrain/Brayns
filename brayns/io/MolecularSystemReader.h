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

#include <brayns/common/loader/Loader.h>
#include <brayns/common/types.h>
#include <string>

namespace brayns
{
/**
 * @brief The ProteinPosition struct contains the ID of each protein as well as
 * its positions
 *        in the system
 */
typedef std::map<size_t, std::string> Proteins;
typedef std::map<size_t, Vector3fs> ProteinPositions;

/**
 * @brief The MolecularSystemReader class loads proteins and meshes according to
 * parameters
 *        defined in a Molecular System Configuration file. This configuration
 * is defined by
 *        the following entries:
 *        - ProteinFolder: Folder containing pdb files
 *        - MeshFolder: Folder containing obj files
 *        - SystemDescriptor: File containing the IDs of the proteins
 *        - ProteinPositions: File containing the position of each protein
 *        - CalciumPositions: File containing the position of each CA atom
 */
class MolecularSystemReader : public Loader
{
public:
    /**
     * @brief Constructor
     * @param geometryParameters Geometry parameters
     */
    MolecularSystemReader(const GeometryParameters& geometryParameters);

    ModelDescriptorPtr importFromFile(
        const std::string& fileName, Scene& scene, const size_t index = 0,
        const Matrix4f& transformation = Matrix4f(),
        const size_t = NO_MATERIAL) final;

    ModelDescriptorPtr importFromBlob(Blob&&, Scene&, const size_t = 0,
                                      const Matrix4f& = Matrix4f(),
                                      const size_t = NO_MATERIAL) final
    {
        throw std::runtime_error("Unsupported");
    }

private:
    bool _createScene(Scene& scene);
    bool _loadConfiguration(const std::string& fileName);
    bool _loadProteins();
    bool _loadPositions();
    void _writePositionstoFile(const std::string& fileName);

    const GeometryParameters& _geometryParameters;
    std::string _proteinFolder;
    std::string _meshFolder;
    std::string _descriptorFilename;
    std::string _positionsFilename;
    std::string _calciumSimulationFolder;
    uint64_t _nbProteins;
    Proteins _proteins;
    ProteinPositions _proteinPositions;
};
}

#endif // MOLECULARSYSTEMREADER_H
