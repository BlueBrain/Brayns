/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <api/reports/IReportData.h>
#include <api/reports/common/SpikeUtils.h>

#include <brain/spikeReportReader.h>

namespace bbploader
{
class SpikeData : public IReportData
{
public:
    SpikeData(std::unique_ptr<brain::SpikeReportReader> report, const std::vector<uint64_t> &gids, float spikeInterval);

    /**
     * @brief Returns the size, in bytes, of a single frame
     *
     * @return size_t
     */
    size_t getFrameSize() const noexcept override;

    /**
     * @brief Returns the report start time
     *
     * @return float
     */
    float getStartTime() const noexcept override;

    /**
     * @brief Returns the report end time
     *
     * @return float
     */
    float getEndTime() const noexcept override;

    /**
     * @brief Returns the report time step
     *
     * @return float
     */
    float getTimeStep() const noexcept override;

    /**
     * @brief Return the reports time unit
     *
     * @return std::string
     */
    std::string getTimeUnit() const noexcept override;

    /**
     * @brief Returns a frame from the report
     *
     * @param frameIndex
     * @return std::vector<float>
     */
    std::vector<float> getFrame(const uint32_t frameIndex) const override;

private:
    const std::unique_ptr<brain::SpikeReportReader> _report;
    const SpikeCalculator _spikeCalculator;
    const float _interval{};
    const std::unordered_map<uint64_t, size_t> _mapping;
};
}
