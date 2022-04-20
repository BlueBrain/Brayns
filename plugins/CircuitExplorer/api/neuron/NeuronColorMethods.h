#pragma once

#include <brayns/utils/EnumUtils.h>

/**
 * @brief Methods availables to color a neuron circuit by (if corresponding data is available as well)
 */
enum class NeuronColorMethods
{
    BY_ETYPE,
    BY_MTYPE,
    BY_LAYER,
    BY_REGION,
    BY_HEMISPHERE,
    BY_MORPHOLOGY,
    BY_MORPHOLOGY_CLASS,
    BY_MORPHOLOGY_SECTION,
    BY_SYNAPSE_CLASS
};

namespace brayns
{
template<>
inline std::vector<std::pair<std::string, NeuronColorMethods>> enumMap()
{
    return {
        {"etype", NeuronColorMethods::BY_ETYPE},
        {"mtype", NeuronColorMethods::BY_MTYPE},
        {"layer", NeuronColorMethods::BY_LAYER},
        {"region", NeuronColorMethods::BY_REGION},
        {"hemisphere", NeuronColorMethods::BY_HEMISPHERE},
        {"morphology", NeuronColorMethods::BY_MORPHOLOGY},
        {"morphology class", NeuronColorMethods::BY_MORPHOLOGY_CLASS},
        {"morphology section", NeuronColorMethods::BY_MORPHOLOGY_SECTION},
        {"synapse class", NeuronColorMethods::BY_SYNAPSE_CLASS}};
}
}
