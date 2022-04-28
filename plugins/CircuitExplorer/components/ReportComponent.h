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

#pragma once

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/DataHandler.h>

#include <api/reports/IColormapIndexer.h>
#include <api/reports/IReportData.h>

class ReportComponent final : public brayns::Component
{
public:
    /**
     * @brief Construct a new Report Component object
     *
     * @param data source of report data
     * @param frameIndexer transforms report data into color map indices
     */
    ReportComponent(std::unique_ptr<IReportData> data, std::unique_ptr<IColormapIndexer> frameIndexer);

    void onStart() override;

    void onPreRender(const brayns::ParametersManager &parameters) override;

private:
    const std::unique_ptr<IReportData> _report;
    const std::unique_ptr<IColormapIndexer> _indexer;

    brayns::OSPBuffer _colors;
    std::vector<uint8_t> _indices;

    // Flag used to force the simulations color update when re-enabling a simulation after it was disabled
    bool _lastEnabledValue{true};
};
