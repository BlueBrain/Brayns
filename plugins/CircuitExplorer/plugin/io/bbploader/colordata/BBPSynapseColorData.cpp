#include "BBPSynapseColorData.h"

#include <stdexcept>

namespace bbploader
{
std::vector<std::string> BBPSynapseColorData::getMethods() const noexcept
{
    return {};
}

std::vector<std::string> BBPSynapseColorData::getMethodVariables(const std::string &method) const
{
    throw std::invalid_argument("Method " + method + " not available for coloring synapses");
    return {};
}

std::vector<std::string> BBPSynapseColorData::getMethodValuesForIDs(
        const std::string &method, const std::vector<uint64_t>& ids) const
{
    throw std::invalid_argument("Method " + method + " not available for coloring synapses");
    return {};
}
} // namespace bbploader
