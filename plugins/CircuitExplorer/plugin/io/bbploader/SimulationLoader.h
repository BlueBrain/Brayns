#pragma once

#include <brayns/engine/Model.h>

#include <plugin/io/BBPLoaderParameters.h>
#include <plugin/io/simulation/SimulationMapping.h>

#include <brion/blueConfig.h>
#include <brain/types.h>

namespace bbploader
{
class SimulationLoader
{
public:
    static void load(const brion::BlueConfig &config,
                     const BBPLoaderParameters &input,
                     const brain::GIDSet &gids,
                     const std::vector<CompartmentStructure> &compartments,
                     brayns::Model &model);
};
}
