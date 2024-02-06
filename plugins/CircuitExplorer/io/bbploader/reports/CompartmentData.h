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

#include <api/reports/IReportData.h>
#include <api/reports/ReportMapping.h>

#include <brion/compartmentReport.h>

namespace bbploader
{
class CompartmentData : public IReportData
{
public:
    explicit CompartmentData(std::unique_ptr<brion::CompartmentReport> report);

    double getStartTime() const noexcept override;
    double getEndTime() const noexcept override;
    double getTimeStep() const noexcept override;
    std::string getTimeUnit() const noexcept override;
    std::vector<float> getFrame(double timestamp) const override;

    /**
     * @brief Computes and returns the report mapping
     *
     * @return std::vector<CellReportMapping>
     */
    std::vector<CellReportMapping> computeMapping() const noexcept;

private:
    std::unique_ptr<brion::CompartmentReport> _report;
};
}
