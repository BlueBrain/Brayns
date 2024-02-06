/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#pragma once

#include <brayns/engine/model/Model.h>

#include <string>

namespace dti
{
/**
 * @brief Builder interface for DTI models
 */
class IDTIBuilder
{
public:
    virtual ~IDTIBuilder() = default;

    /**
     * @brief Resets the builder to initial state
     */
    virtual void reset() = 0;

    /**
     * @brief Process the step to reads the gid to row mapping file
     * @param path Path to the gid to row mapping file
     */
    virtual void readGidRowFile(const std::string &path) = 0;

    /**
     * @brief Process the step to read the streamline geometry file
     * @param path Path to the streamline geometry file
     */
    virtual void readStreamlinesFile(const std::string &path) = 0;

    /**
     * @brief Process the step to build the geometry from stored data
     * @param radius Radius of the streamline cylinder geoemtries
     * @param model Target model where the DTIComponent with the geometry will be added
     */
    virtual void buildGeometry(float radius, brayns::Model &model) = 0;

    /**
     * @brief Process the step to generate a simulation renderer from a given circuit and stored data
     * @param path Path to a BlueConfig based circuit with a spike report
     * @param spikeDecayTime Decay time (in report time unit) for a spike
     * @param model Target model where the SpikeReportComponent will be addded
     */
    virtual void buildSimulation(const std::string &path, float spikeDecayTime, brayns::Model &model) = 0;
};
}
