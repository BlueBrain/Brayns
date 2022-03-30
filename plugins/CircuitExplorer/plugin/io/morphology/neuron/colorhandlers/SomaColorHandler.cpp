#include "SomaColorHandler.h"

#include <plugin/api/ColorUtils.h>

SomaColorHandler::SomaColorHandler(NeuronColorData &colorData, SomaCircuitComponent &circuit)
 : _colorData(colorData)
 , _circuit(circuit)
{
}

void SomaColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    _circuit.setColorById(colorMap);
}

void SomaColorHandler::updateColorById(const std::vector<brayns::Vector4f> &colors)
{
    _circuit.setColorById(colors);
}

void SomaColorHandler::updateSingleColor(const brayns::Vector4f &color)
{
    _circuit.setColor(color);
}

void SomaColorHandler::updateColor(const std::string &method, const std::vector<ColoringInformation> &vars)
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

void SomaColorHandler::updateIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices)
{
    _circuit.setIndexedColor(color, indices);
}

void SomaColorHandler::_colorWithInput(const std::string &method, const std::vector<ColoringInformation> &vars)
{
    if(method == _colorData.morphologySectionMethodName())
    {
        // Search for soma
        for(const auto &variable : vars)
        {
            const auto &sectionName = variable.variable;
            const auto &sectionColor = variable.color;
            auto sectionType = brayns::stringToEnum<NeuronSection>(sectionName);
            if(sectionType == NeuronSection::SOMA)
            {
                _circuit.setColor(sectionColor);
                break;
            }
        }
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

void SomaColorHandler::_colorAll(const std::string &method)
{
    if(method == _colorData.morphologySectionMethodName())
    {
        ColorRoulette roulette;
        _circuit.setColor(roulette.getNextColor());
    }
    else
    {
        ColorDeck deck;
        const auto &ids = _circuit.getIDs();
        const auto perIdValues = _colorData.getMethodValuesForIDs(method, ids);
        std::vector<brayns::Vector4f> result (ids.size());
        for(size_t i = 0; i < ids.size(); ++i)
        {
            const auto &value = perIdValues[i];
            const auto &color = deck.getColorForKey(value);
            result[i] = color;
        }
        _circuit.setColorById(result);
    }
}
