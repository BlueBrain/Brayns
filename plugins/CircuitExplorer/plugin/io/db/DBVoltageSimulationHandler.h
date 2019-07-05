/* Copyright (c) 2018-2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#ifndef DBVoltageSimulationHandler_H
#define DBVoltageSimulationHandler_H

#include <api/CircuitExplorerParams.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <brayns/engine/Scene.h>
#include <brion/brion.h>

#include <pqxx/pqxx>

/**
 * @brief The DBVoltageSimulationHandler class handles simulation frames for the
 * current circuit. Frames are stored in a memory mapped file that is accessed
 * according to a specified timestamp. The DBVoltageSimulationHandler class is
 * in charge of keeping the handle to the memory mapped file.
 */
class DBVoltageSimulationHandler : public brayns::AbstractSimulationHandler
{
public:
    /**
     * @brief Default constructor
     * @param geometryParameters Geometry parameters
     * @param reportSource path to report source
     * @param gids GIDS to load
     */
    DBVoltageSimulationHandler(const std::string& connectionString);
    DBVoltageSimulationHandler(const DBVoltageSimulationHandler& rhs);
    ~DBVoltageSimulationHandler();

    void* getFrameData(const uint32_t frame) final;

    bool isReady() const final { return true; }

    brayns::AbstractSimulationHandlerPtr clone() const final;

private:
    std::vector<float> _loadCompartmentSimulation(const uint64_t frame);
    pqxx::connection _connection;
};

#endif // DBVoltageSimulationHandler_H
