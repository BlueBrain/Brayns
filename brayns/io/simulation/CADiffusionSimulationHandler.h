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

#ifndef CADIFFUSIONSIMULATIONHANDLER_H
#define CADIFFUSIONSIMULATIONHANDLER_H

#include <brayns/api.h>
#include <brayns/common/types.h>

namespace brayns
{
/**
 * @brief The CADiffusionSimulationHandler class handles simulation frames for
 *        Calcium diffusion. Frames are stored in ASCII files containing
 *        coordinates for CA atoms. Each frame is in a different file. The
 *        format of the frame is X Y Z values stored as text. For example:
 *        215.388692 996.594668 338.199478
 */
class CADiffusionSimulationHandler
{
public:
    /**
     * @brief Default constructor
     * @param simulationFolder Folder containing files with the CA atom
     *        positions. Files must have a .dat extension.
     */
    CADiffusionSimulationHandler(MaterialManager& materialManager,
                                 const std::string& simulationFolder);

    /**
     * @brief setFrame Sets the frame to load
     * @param model Model to be populated with spheres. When setFrame is called
     * for the first time, spheres are created. Otherwise, sphere
     * positions are updated with the new values
     * @param frame Frame to load
     */
    void setFrame(Model& model, const size_t frame);

    /**
     * @return Returns the number of frames for the current simulation
     */
    uint64_t getNbFrames() const { return _simulationFiles.size(); }
private:
    bool _loadCalciumPositions(const size_t frame);

    MaterialManager& _materialManager;
    std::map<size_t, std::string> _simulationFiles;
    Vector3fs _calciumPositions;
    size_t _currentFrame{std::numeric_limits<size_t>::max()};
    bool _spheresCreated{false};
};
}
#endif // CADIFFUSIONSIMULATIONHANDLER_H
