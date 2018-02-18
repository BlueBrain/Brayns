/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
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

#ifndef SPIKESIMULATIONHANDLER_H
#define SPIKESIMULATIONHANDLER_H

#include <brayns/api.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/types.h>

namespace brayns
{
/**
 * @brief The SpikeSimulationHandler class handles simulation frames for the
 * current circuit.
 *        Frames are stored in a memory mapped file that is accessed according
 * to a specified
 *        timestamp. The SpikeSimulationHandler class is in charge of keeping
 * the handle to the
 *        memory mapped file.
 */
class SpikeSimulationHandler : public AbstractSimulationHandler
{
public:
    /**
     * @brief Default constructor
     * @param geometryParameters Geometry parameters
     */
    SpikeSimulationHandler(const GeometryParameters& geometryParameters);

    /**
     * @brief Returns a pointer to requested frame in the memory mapped file.
     * @return Pointer to given frame
     */
    void* getFrameData(uint32_t frame) final;
};
}

#endif // SPIKESIMULATIONHANDLER_H
