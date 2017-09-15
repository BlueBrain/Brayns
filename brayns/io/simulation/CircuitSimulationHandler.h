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

#ifndef CIRCUITSIMULATIONHANDLER_H
#define CIRCUITSIMULATIONHANDLER_H

#include <brayns/api.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/types.h>
#include <brion/brion.h>

namespace brayns
{
typedef std::shared_ptr<brion::CompartmentReport> CompartmentReportPtr;

/**
 * @brief The CircuitSimulationHandler class handles simulation frames for the
 * current circuit. Frames are stored in a memory mapped file that is accessed
 * according to a specified timestamp. The CircuitSimulationHandler class is in
 * charge of keeping the handle to the memory mapped file.
 */
class CircuitSimulationHandler : public AbstractSimulationHandler
{
public:
    /**
     * @brief Default contructor
     * @param geometryParameters Geometry parameters
     * @param reportSource path to report source
     * @param gids GIDS to load
     */
    CircuitSimulationHandler(const GeometryParameters& geometryParameters,
                             const std::string& reportSource,
                             const brion::GIDSet& gids);
    ~CircuitSimulationHandler();

    /**
     * @brief Returns a pointer to the current frame in the memory mapped file.
     * @return Pointer to given frame
     */
    void* getFrameData() final;

    CompartmentReportPtr getCompartmentReport() { return _compartmentReport; }
private:
    CompartmentReportPtr _compartmentReport;
    double _beginFrame;
    double _endFrame;
    double _timeBetweenFrames;
    std::shared_ptr<floats> _frameValues;
};
}

#endif // CIRCUITSIMULATIONHANDLER
