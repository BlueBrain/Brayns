#include "PrimitiveColorHandler.h"

#include <plugin/api/ColorUtils.h>

PrimitiveColorHandler::PrimitiveColorHandler(NeuronColorData &colorData, MorphologyCircuitComponent &circuit)
 : _colorData(colorData)
 , _circuit(circuit)
{
}

void PrimitiveColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    _circuit.setColorById(colorMap);
}

void PrimitiveColorHandler::updateSingleColor(const brayns::Vector4f &color)
{
    _circuit.setColor(color);
}

void PrimitiveColorHandler::updateColor(const std::string &method, const std::vector<ColoringInformation> &vars)
{
    if(!vars.empty())
    {
        _colorWithInput(method, vars);
    }
    else
    {
        _colorAll(method);
    }
}

void PrimitiveColorHandler::_colorWithInput(const std::string &method, const std::vector<ColoringInformation> &vars)
{
    if(method == _colorData.morphologySectionMethodName())
    {
        std::vector<std::pair<NeuronSection, brayns::Vector4f>> sectionColorMap;
        sectionColorMap.reserve(vars.size());

        for(const auto &variable : vars)
        {
            const auto &sectionName = variable.variable;
            const auto &sectionColor = variable.color;
            auto sectionType = brayns::stringToEnum<NeuronSection>(sectionName);
            sectionColorMap.push_back(std::make_pair(sectionType, sectionColor));
        }

        _circuit.setColorBySection(sectionColorMap);
    }
    else
    {
        const auto &ids = _circuit.getIDs();
        const auto perIdValues = _colorData.getMethodValuesForIDs(method, ids);

        std::unordered_map<std::string, brayns::Vector4f> groupedVariables;
        for(const auto &variable : vars)
        {
            const auto &name = variable.variable;
            const auto &color = variable.color;
            groupedVariables[name] = color;
        }

        std::map<uint64_t, brayns::Vector4f> colorMap;
        for(size_t i = 0; i < perIdValues.size(); ++i)
        {
            const auto id = ids[i];
            const auto &value = perIdValues[i];

            auto it = groupedVariables.find(value);
            if(it == groupedVariables.end())
            {
                continue;
            }

            const auto &color = it->second;
            colorMap[id] = color;
        }

        _circuit.setColorById(colorMap);
    }
}

void PrimitiveColorHandler::_colorAll(const std::string &method)
{
    if(method == _colorData.morphologySectionMethodName())
    {
        ColorRoulette roulette;
        const std::vector<std::pair<NeuronSection, brayns::Vector4f>> sectionColorMap =
        {
            std::make_pair(NeuronSection::SOMA, roulette.getNextColor()),
            std::make_pair(NeuronSection::AXON, roulette.getNextColor()),
            std::make_pair(NeuronSection::DENDRITE, roulette.getNextColor()),
            std::make_pair(NeuronSection::APICAL_DENDRITE, roulette.getNextColor())
        };
        _circuit.setColorBySection(sectionColorMap);
    }
    else
    {
        ColorDeck deck;
        const auto &ids = _circuit.getIDs();
        const auto perIdValues = _colorData.getMethodValuesForIDs(method, ids);
        std::vector<brayns::Vector4f> result (ids.size());
        for(size_t i = 0; i < ids.size(); ++i)
        {
            const auto id = ids[i];
            const auto &value = perIdValues[i];
            const auto &color = deck.getColorForKey(value);
            result[i] = color;
        }
        _circuit.setColorById(result);
    }
}

void PrimitiveColorHandler::updateSimulationColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices)
{
    _circuit.setSimulationColor(color, indices);
}
