#include "VasculatureColorHandler.h"

#include <plugin/api/ColorUtils.h>

VasculatureColorHandler::VasculatureColorHandler(VasculatureColorData &colorData, VasculatureComponent &vasculature)
 : _colorData(colorData)
 , _vasculature(vasculature)
{
}

void VasculatureColorHandler::updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap)
{
    _vasculature.setColorById(colorMap);
}

void VasculatureColorHandler::updateSingleColor(const brayns::Vector4f &color)
{
    _vasculature.setColor(color);
}

void VasculatureColorHandler::updateColor(const std::string &method, const std::vector<ColoringInformation> &vars)
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

void VasculatureColorHandler::updateSimulationColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices)
{
    _vasculature.setSimulationColor(color, indices);
}

void VasculatureColorHandler::_colorWithInput(const std::string &method, const std::vector<ColoringInformation> &vars)
{
    if(method == _colorData.vasculatureSectionMethodName())
    {
        std::vector<std::pair<VasculatureSection, brayns::Vector4f>> sectionColorMap;
        sectionColorMap.reserve(vars.size());

        for(const auto &variable : vars)
        {
            const auto &sectionName = variable.variable;
            const auto &sectionColor = variable.color;
            auto sectionType = brayns::stringToEnum<VasculatureSection>(sectionName);
            sectionColorMap.push_back(std::make_pair(sectionType, sectionColor));
        }

        _vasculature.setColorBySection(sectionColorMap);
    }
    else
    {
        const auto &ids = _vasculature.getIDs();
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

        _vasculature.setColorById(colorMap);
    }
}

void VasculatureColorHandler::_colorAll(const std::string &method)
{
    if(method == _colorData.vasculatureSectionMethodName())
    {
        ColorRoulette roulette;
        const std::vector<std::pair<VasculatureSection, brayns::Vector4f>> sectionColorMap =
        {
            std::make_pair(VasculatureSection::ARTERY, roulette.getNextColor()),
            std::make_pair(VasculatureSection::VEIN, roulette.getNextColor()),
            std::make_pair(VasculatureSection::ARTERIOLE, roulette.getNextColor()),
            std::make_pair(VasculatureSection::VENULE, roulette.getNextColor()),
            std::make_pair(VasculatureSection::ARTERIAL_CAPILLARY, roulette.getNextColor()),
            std::make_pair(VasculatureSection::VENOUS_CAPILLARY, roulette.getNextColor()),
            std::make_pair(VasculatureSection::TRANSITIONAL, roulette.getNextColor())
        };
        _vasculature.setColorBySection(sectionColorMap);
    }
    else
    {
        ColorDeck deck;
        const auto &ids = _vasculature.getIDs();
        const auto perIdValues = _colorData.getMethodValuesForIDs(method, ids);
        std::vector<brayns::Vector4f> result (ids.size());
        for(size_t i = 0; i < ids.size(); ++i)
        {
            const auto id = ids[i];
            const auto &value = perIdValues[i];
            const auto &color = deck.getColorForKey(value);
            result[i] = color;
        }
        _vasculature.setColorById(result);
    }
}
