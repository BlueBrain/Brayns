#include "SimulationLoader.h"

#include <brayns/common/Log.h>

#include <plugin/io/bbploader/simulation/components/CompartmentReportComponent.h>
#include <plugin/io/bbploader/simulation/components/SpikeReportComponent.h>

namespace bbploader
{
void SimulationLoader::load(const brion::BlueConfig &config,
                            const BBPLoaderParameters &params,
                            const brain::GIDSet& gids,
                            const std::vector<CompartmentStructure> &compartments,
                            brayns::Model &model)
{
    const auto reportType = params.report_type;
    if (reportType == bbploader::SimulationType::COMPARTMENT)
    {
        const auto &reportName = params.report_name;
        const auto reportPath = config.getReportSource(reportName).getPath();
        const brion::URI uri (reportPath);
        auto report = std::make_unique<brion::CompartmentReport>(uri, brion::AccessMode::MODE_READ, gids);
        model.addComponent<bbploader::CompartmentReportComponent>(std::move(report), compartments);
    }
    else if(reportType == bbploader::SimulationType::SPIKES)
    {
        const auto reportPath = config.getSpikeSource().getPath();
        const brion::URI uri (reportPath);
        auto report = std::make_unique<brion::SpikeReport>(uri, brion::AccessMode::MODE_READ);
        model.addComponent<bbploader::SpikeReportComponent>(std::move(report), compartments);
    }
}
}
