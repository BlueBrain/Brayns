#include "GIDLoadList.h"

#include <brion/compartmentReport.h>

namespace bbploader
{
brain::GIDSet GIDLoadList::compute(const brion::BlueConfig &config,
                                   const brain::Circuit &circuit,
                                   const BBPLoaderParameters &input)
    {
        brain::GIDSet allGids;
        allGids = fromParameters(config, circuit, input);
        allGids = fromSimulation(config, input, allGids);
        allGids = fromPercentage(allGids, input.percentage);
        return allGids;
    }

brain::GIDSet GIDLoadList::fromParameters(const brion::BlueConfig &config,
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

brain::GIDSet GIDLoadList::fromSimulation(const brion::BlueConfig &config,
                                          const BBPLoaderParameters &input,
                                          const brain::GIDSet &src)
{
    const auto reportType = input.report_type;
    if(reportType == bbploader::SimulationType::COMPARTMENT)
    {
        const auto &reportName = input.report_name;
        const auto reportPath = config.getReportSource(reportName).getPath();
        const brion::URI uri (reportPath);
        const brion::CompartmentReport report (uri, brion::AccessMode::MODE_READ, src);
        return report.getGIDs();
    }

    return src;
}

brain::GIDSet GIDLoadList::fromPercentage(const brain::GIDSet &src, const float percentage)
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
