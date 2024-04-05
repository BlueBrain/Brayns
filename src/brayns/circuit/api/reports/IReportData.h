/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include <string>
#include <vector>

/**
 * @brief Interface to access report data
 */
class IReportData
{
public:
    virtual ~IReportData() = default;

    /**
     * @brief Returns the report start time
     *
     * @return float
     */
    virtual double getStartTime() const noexcept = 0;

    /**
     * @brief Returns the report end time
     *
     * @return float
     */
    virtual double getEndTime() const noexcept = 0;

    /**
     * @brief Returns the report time step
     *
     * @return float
     */
    virtual double getTimeStep() const noexcept = 0;

    /**
     * @brief Return the reports time unit
     *
     * @return std::string
     */
    virtual std::string getTimeUnit() const noexcept = 0;

    /**
     * @brief Returns a frame from the report
     *
     * @param timestamp Timestamp of the frame to load
     * @return std::vector<float>
     */
    virtual std::vector<float> getFrame(double timestamp) const = 0;
};
