/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <brain/brain.h>
#include <brion/brion.h>

#include <brayns/network/entrypoint/Entrypoint.h>

#include <messages/CIGetReportInfoMessage.h>

class ReportInfoRetriever
{
public:
    static auto getReportInfo(const CIGetReportInfoParams& params)
    {
        CIGetReportInfoResult result;

        // Read config file
        brion::BlueConfig config(params.path);

        // Get all report names
        auto& reports = config.getSectionNames(brion::CONFIGSECTION_REPORT);

        // Find required report
        auto i = std::find(reports.begin(), reports.end(), params.report);
        if (i == reports.end())
        {
            throw brayns::EntrypointException(1, "Report not found");
        }

        // Get report
        auto path = config.getReportSource(params.report);
        auto mode = brion::AccessMode::MODE_READ;
        brion::CompartmentReport report(path, mode);

        // Extract info
        result.start_time = report.getStartTime();
        result.end_time = report.getEndTime();
        result.time_step = report.getTimestep();
        result.data_unit = report.getDataUnit();
        result.time_unit = report.getTimeUnit();
        result.frame_count = report.getFrameCount();
        result.frame_size = report.getFrameSize();

        return result;
    }
};

class CIGetReportInfoEntrypoint
    : public brayns::Entrypoint<CIGetReportInfoParams, CIGetReportInfoResult>
{
public:
    virtual std::string getName() const override
    {
        return "ci-get-report-info";
    }

    virtual std::string getDescription() const override
    {
        return "Return information about a specific report from a circuit";
    }

    virtual void onRequest(const Request& request) override
    {
        auto params = request.getParams();
        auto result = ReportInfoRetriever::getReportInfo(params);
        request.reply(result);
    }
};