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

#ifndef VOLTAGESIMULATIONHANDLER_H
#define VOLTAGESIMULATIONHANDLER_H

#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <plugin/api/CircuitExplorerParams.h>

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <brayns/engine/Scene.h>
#include <brion/brion.h>

typedef std::shared_ptr<brion::CompartmentReport> CompartmentReportPtr;

/**
 * @brief The VoltageSimulationHandler class handles simulation frames for the
 * current circuit. Frames are stored in a memory mapped file that is accessed
 * according to a specified timestamp. The VoltageSimulationHandler class is in
 * charge of keeping the handle to the memory mapped file.
 */
class VoltageSimulationHandler : public brayns::AbstractSimulationHandler
{
public:
    /**
     * @brief Default constructor
     * @param geometryParameters Geometry parameters
     * @param reportSource path to report source
     * @param gids GIDS to load
     */
    VoltageSimulationHandler(const std::string& reportPath,
                             const brion::GIDSet& gids,
                             const bool synchronousMode = false);
    VoltageSimulationHandler(const VoltageSimulationHandler& rhs);
    ~VoltageSimulationHandler();

    void* getFrameDataImpl(const uint32_t frame) final;

    const std::string& getReportPath() const { return _reportPath; }
    CompartmentReportPtr getReport() const { return _compartmentReport; }
    bool isSynchronized() const { return _synchronousMode; }
    bool isReady() const final;

    brayns::AbstractSimulationHandlerPtr clone() const final;

private:
    void _triggerLoading(const uint32_t frame);
    bool _isFrameLoaded() const;
    bool _makeFrameReady(const uint32_t frame);
    bool _synchronousMode{false};

    std::string _reportPath;
    CompartmentReportPtr _compartmentReport;
    std::future<brion::Frame> _currentFrameFuture;
    std::map<uint64_t, std::vector<float>> _frames;
    bool _ready{false};
};

#endif // VOLTAGESIMULATIONHANDLER_H
