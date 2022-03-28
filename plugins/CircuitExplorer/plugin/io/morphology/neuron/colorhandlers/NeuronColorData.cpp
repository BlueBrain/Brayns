#include "NeuronColorData.h"

#include <plugin/io/morphology/neuron/NeuronSection.h>

std::vector<std::string> NeuronColorData::getMethods() const noexcept
{
    auto result = getCircuitMethods();
    result.push_back(morphologySectionMethodName());
    return result;
}

std::vector<std::string> NeuronColorData::getMethodVariables(const std::string &method) const
{
    if(method == morphologySectionMethodName())
    {
        return brayns::enumNames<NeuronSection>();
    }
    else
    {
        return getCircuitMethodVariables(method);
    }
}

std::string NeuronColorData::morphologySectionMethodName() const noexcept
{
    return "morphology section";
}
