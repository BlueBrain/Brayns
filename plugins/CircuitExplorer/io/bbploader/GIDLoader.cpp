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

#include "GIDLoader.h"

#include <brion/compartmentReport.h>

namespace
{
class TargetGIDReader
{
public:
    static brain::GIDSet read(const brain::Circuit &circuit, const std::vector<std::string> &targets)
    {
        brain::GIDSet result;
        for (const auto &target : targets)
        {
            auto tempGids = circuit.getGIDs(target);
            result.insert(tempGids.begin(), tempGids.end());
        }
        return result;
    }
};

class BaseGIDsFetcher
{
public:
    static brain::GIDSet fromParameters(
        const brion::BlueConfig &config,
        const brain::Circuit &circuit,
        const BBPLoaderParameters &input)
    {
        if (input.gids.has_value())
        {
            auto &gids = *input.gids;
            return brain::GIDSet(gids.begin(), gids.end());
        }

        if (input.targets.has_value())
        {
            auto &targets = *input.targets;
            return TargetGIDReader::read(circuit, targets);
        }

        if (auto circuitTarget = config.getCircuitTarget(); !circuitTarget.empty())
        {
            return TargetGIDReader::read(circuit, {circuitTarget});
        }

        return circuit.getGIDs();
    };
};

class ReportGIDIntersector
{
public:
    static brain::GIDSet intersect(
        const brion::BlueConfig &config,
        const std::string &reportName,
        const brain::GIDSet &source)
    {
        auto reportPath = config.getReportSource(reportName).getPath();
        const brion::URI uri(reportPath);
        const brion::CompartmentReport report(uri, brion::AccessMode::MODE_READ, source);
        return report.getGIDs();
    }
};

class GIDSampler
{
public:
    static brain::GIDSet subSample(const brain::GIDSet &input, float percentage)
    {
        auto nbGids = static_cast<float>(input.size());
        const auto expectedSize = static_cast<size_t>(nbGids * percentage);
        auto skipFactor = static_cast<size_t>(nbGids / static_cast<float>(expectedSize));
        skipFactor = std::max(skipFactor, 1ul);
        brain::GIDSet subsampled;
        auto it = subsampled.begin();
        auto allIt = input.begin();
        while (allIt != input.end())
        {
            subsampled.insert(it, *allIt);
            ++it;
            size_t counter{0};
            while (counter++ < skipFactor && allIt != input.end())
            {
                ++allIt;
            }
        }

        return subsampled;
    }
};
}

namespace bbploader
{
brain::GIDSet GIDLoader::compute(
    const brion::BlueConfig &config,
    const brain::Circuit &circuit,
    const BBPLoaderParameters &input)
{
    auto inputGidList = input.gids.has_value();
    auto baseGids = BaseGIDsFetcher::fromParameters(config, circuit, input);

    if (input.report_type == bbploader::ReportType::Compartment)
    {
        baseGids = ReportGIDIntersector::intersect(config, input.report_name, baseGids);
    }

    if (!inputGidList && input.percentage < 1.f)
    {
        baseGids = GIDSampler::subSample(baseGids, input.percentage);
    }

    if (baseGids.empty())
    {
        throw std::runtime_error("Empty list of GIDs computed");
    }

    return baseGids;
}
}
