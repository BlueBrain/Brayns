/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/BlueBrain/Brayns>
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

#pragma once

#include <brayns/common/simulation/AbstractSimulationHandler.h>

#include <brion/brion.h>

/**
 * @brief The CompartmentHandler class handles BBP's internal format
 *        compartment report frame access.
 */
class CompartmentHandler : public brayns::AbstractSimulationHandler
{
public:
    CompartmentHandler(const std::string& path,
                       const std::shared_ptr<brion::CompartmentReport>& report);
    CompartmentHandler(const CompartmentHandler&);

    brayns::AbstractSimulationHandlerPtr clone() const final;

    bool isReady() const final;

    std::vector<float> getFrameDataImpl(const uint32_t frame) final;

private:
    const std::string _path;
    const std::shared_ptr<brion::CompartmentReport> _report;
    bool _ready{false};
};
