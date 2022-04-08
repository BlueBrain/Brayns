/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "GIDLoader.h"

#include <brion/compartmentReport.h>

namespace bbploader
{
brain::GIDSet
    GIDLoader::compute(const brion::BlueConfig &config, const brain::Circuit &circuit, const BBPLoaderParameters &input)
{
    brain::GIDSet allGids;
    allGids = _fromParameters(config, circuit, input);
    allGids = _fromSimulation(config, input, allGids);
    allGids = _fromPercentage(allGids, input.percentage);

    if (allGids.empty())
    {
        throw std::runtime_error("No GIDs were selected with the input parameters");
    }

    return allGids;
}

brain::GIDSet GIDLoader::_fromParameters(
    const brion::BlueConfig &config,
    const brain::Circuit &circuit,
    const BBPLoaderParameters &input)
{
    if (!input.gids.empty())
    {
        return brain::GIDSet(input.gids.begin(), input.gids.end());
    }

    brain::GIDSet result;
    std::vector<std::string> targets;
    if (!input.targets.empty())
    {
        targets = input.targets;
    }
    else if (const auto defaultTarget = config.getCircuitTarget(); !defaultTarget.empty())
    {
        targets = {defaultTarget};
    }

    brain::GIDSet allGids;
    if (!targets.empty())
    {
        for (const auto &target : targets)
        {
            const auto tempGids = circuit.getGIDs(target);
            allGids.insert(tempGids.begin(), tempGids.end());
        }
    }
    else
    {
        allGids = circuit.getGIDs();
    }

    return allGids;
}

brain::GIDSet GIDLoader::_fromSimulation(
    const brion::BlueConfig &config,
    const BBPLoaderParameters &input,
    const brain::GIDSet &src)
{
    const auto reportType = input.report_type;
    if (reportType == bbploader::ReportType::COMPARTMENT)
    {
        const auto &reportName = input.report_name;
        const auto reportPath = config.getReportSource(reportName).getPath();
        const brion::URI uri(reportPath);
        const brion::CompartmentReport report(uri, brion::AccessMode::MODE_READ, src);
        return report.getGIDs();
    }

    return src;
}

brain::GIDSet GIDLoader::_fromPercentage(const brain::GIDSet &src, const float percentage)
{
    if (percentage >= 1.f)
    {
        return src;
    }

    const auto expectedSize = static_cast<size_t>(src.size() * percentage);
    const auto skipFactor = static_cast<size_t>(static_cast<float>(src.size()) / static_cast<float>(expectedSize));
    brain::GIDSet finalList;
    auto it = finalList.begin();
    auto allIt = src.begin();
    while (allIt != src.end())
    {
        finalList.insert(it, *allIt);
        ++it;
        size_t counter{0};
        while (counter++ < skipFactor && allIt != src.end())
            ++allIt;
    }

    return finalList;
}
}
