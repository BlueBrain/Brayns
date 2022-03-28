#pragma once

#include <plugin/io/BBPLoaderParameters.h>

#include <brion/blueConfig.h>
#include <brain/circuit.h>

namespace bbploader
{
class GIDLoadList
{
public:
    static brain::GIDSet compute(const brion::BlueConfig &config,
                                 const brain::Circuit &circuit,
                                 const BBPLoaderParameters &input);

private:
    static brain::GIDSet fromParameters(const brion::BlueConfig &config,
                                        const brain::Circuit &circuit,
                                        const BBPLoaderParameters &input);

    static brain::GIDSet fromSimulation(const brion::BlueConfig &config,
                                        const BBPLoaderParameters &input,
                                        const brain::GIDSet &src);

    static brain::GIDSet fromPercentage(const brain::GIDSet &src,
                                        const float percentage);
};
}
