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

#include "DBVoltageSimulationHandler.h"
#include "DBConnector.h"

#include <common/log.h>

#include <brayns/common/Timer.h>
#include <brayns/parameters/AnimationParameters.h>

DBVoltageSimulationHandler::DBVoltageSimulationHandler(
    const std::string& connectionString)
    : brayns::AbstractSimulationHandler()
    , _connection(connectionString)
{
    try
    {
        const auto sql =
            "SELECT time_step, end_time, time_unit, frame_size FROM "
            "simulation_report WHERE guid=0";

        pqxx::work transaction(_connection);
        auto res = transaction.exec(sql);
        for (auto c = res.begin(); c != res.end(); ++c)
        {
            _dt = c[0].as<float>();
            _nbFrames = c[1].as<float>() / _dt;
            _unit = c[2].as<std::string>();
            _frameSize = c[3].as<size_t>();
        }
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }

    PLUGIN_INFO << "-----------------------------------------------------------"
                << std::endl;
    PLUGIN_INFO << "Voltage simulation information" << std::endl;
    PLUGIN_INFO << "----------------------" << std::endl;
    PLUGIN_INFO << "End time             : " << _nbFrames * _dt << std::endl;
    PLUGIN_INFO << "Steps between frames : " << _dt << std::endl;
    PLUGIN_INFO << "Number of frames     : " << _nbFrames << std::endl;
    PLUGIN_INFO << "Frame size           : " << _frameSize << std::endl;
    PLUGIN_INFO << "-----------------------------------------------------------"
                << std::endl;
}

DBVoltageSimulationHandler::DBVoltageSimulationHandler(
    const DBVoltageSimulationHandler& rhs)
    : brayns::AbstractSimulationHandler(rhs)
{
}

DBVoltageSimulationHandler::~DBVoltageSimulationHandler() {}

brayns::AbstractSimulationHandlerPtr DBVoltageSimulationHandler::clone() const
{
    return std::make_shared<DBVoltageSimulationHandler>(*this);
}

std::vector<float> DBVoltageSimulationHandler::_loadCompartmentSimulation(
    const uint64_t frame)
{
    std::vector<float> values;
    const brayns::Timer chrono0;
    try
    {
        const std::string sql =
            "SELECT voltages FROM "
            "simulation_compartments WHERE frame=" +
            std::to_string(frame) + " ORDER BY guid";

        pqxx::work transaction(_connection);
        auto res = transaction.exec(sql);
        for (auto c = res.begin(); c != res.end(); ++c)
        {
            const auto currentSize = values.size();
            const pqxx::binarystring voltages(c[0]);
            values.resize(currentSize + voltages.size() / sizeof(float));
            memcpy(&values[currentSize], voltages.data(), voltages.size());
        }
    }
    catch (pqxx::sql_error& e)
    {
        PLUGIN_THROW(e.what());
    }
    PLUGIN_TIMER(chrono0.elapsed(), "loadCompartmentSimulationFromDB");
    return values;
}

void* DBVoltageSimulationHandler::getFrameData(const uint32_t frame)
{
    const auto boundedFrame = _getBoundedFrame(frame);

    if (_currentFrame != boundedFrame)
    {
        _frameData = _loadCompartmentSimulation(boundedFrame);
        _currentFrame = boundedFrame;
    }
    return _frameData.data();
}
