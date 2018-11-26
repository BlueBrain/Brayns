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
#include <set>
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
    MolecularSystemReader(Scene& scene);

    std::vector<std::string> getSupportedExtensions() const final;
    std::string getName() const final;
    PropertyMap getProperties() const final;

    bool isSupported(const std::string& filename,
                     const std::string& extension) const final;
    ModelDescriptorPtr importFromFile(const std::string& fileName,
                                      const LoaderProgress& callback,
                                      const PropertyMap& properties,
                                      const size_t index = 0,
                                      const size_t = NO_MATERIAL) const final;

    ModelDescriptorPtr importFromBlob(Blob&&, const LoaderProgress&,
                                      const PropertyMap& properties
                                          BRAYNS_UNUSED,
                                      const size_t, const size_t) const final
    {
        throw std::runtime_error("Loading from blob not supported");
    }

private:
    struct LoaderData
    {
        std::string _proteinFolder;
        std::string _meshFolder;
        std::string _descriptorFilename;
        std::string _positionsFilename;
        std::string _calciumSimulationFolder;
        uint64_t _nbProteins;
        Proteins _proteins;
        ProteinPositions _proteinPositions;
        LoaderProgress _callback;
    };

    bool _createScene(LoaderData& data,
                      const PropertyMap& properties) const;
    bool _loadConfiguration(const std::string& fileName,
                            LoaderData& data) const;
    bool _loadProteins(LoaderData& data) const;
    bool _loadPositions(LoaderData& data) const;
    void _writePositionstoFile(const std::string& fileName,
                               const LoaderData& data) const;
};
}

#endif // MOLECULARSYSTEMREADER_H
