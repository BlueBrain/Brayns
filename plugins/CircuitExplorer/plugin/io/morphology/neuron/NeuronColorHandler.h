/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <set>

#include <plugin/api/CircuitColorHandler.h>
#include <plugin/api/ColorUtils.h>
#include <plugin/io/morphology/neuron/NeuronMaterialMap.h>
#include <plugin/io/morphology/neuron/NeuronSection.h>

/**
 * @brief The NeuronColorHandler is the base class for CircuitColorHandlers of
 *        neuron and astrocyte based circuits.
 */

namespace
{
constexpr char METHOD_BY_SECTION[] = "morphology section";
}

template <typename IDListType>
class NeuronColorHandler : public CircuitColorHandler
{
public:
    void _setElementsImpl(const std::vector<uint64_t>& ids,
                          std::vector<ElementMaterialMap::Ptr>&& elements) final
    {
        _ids = IDListType(ids.begin(), ids.end());
        _elements = std::move(elements);
    }

    std::vector<std::string> _getMethodsImpl() const final
    {
        auto extra = _getExtraMethods();
        extra.push_back(METHOD_BY_SECTION);
        return extra;
    }

    std::vector<std::string> _getMethodVariablesImpl(
        const std::string& method) const final
    {
        if (method == METHOD_BY_SECTION)
            return brayns::enumNames<NeuronSection>();

        const auto methodValues = _getValuesForMethod(method);
        const std::set<std::string> uniqueValues(methodValues.begin(),
                                                 methodValues.end());
        return std::vector<std::string>(uniqueValues.begin(),
                                        uniqueValues.end());
    }

    void _updateColorByIdImpl(
        const std::map<uint64_t, brayns::Vector4f>& colorMap) final
    {
        if (!colorMap.empty())
        {
            auto it = colorMap.begin();
            auto idIt = _ids.begin();
            size_t index = 0;
            while (it != colorMap.end() && idIt != _ids.end())
            {
                const auto id = it->first;
                if (id > *_ids.rbegin())
                    throw std::invalid_argument(
                        "Requested coloring GID '" + std::to_string(id) +
                        "' is beyond the highest GID loaded '" +
                        std::to_string(*_ids.rbegin()) + "'");

                while (id > *idIt && idIt != _ids.end())
                {
                    ++idIt;
                    ++index;
                }

                if (id == *idIt)
                    _elements[index]->setColor(_model, it->second);

                ++it;
            }
        }
        else
        {
            ColorRoulette r;
            for (auto& element : _elements)
                element->setColor(_model, r.getNextColor());
        }
    }

    void _updateSingleColorImpl(const brayns::Vector4f& color) final
    {
        for (auto& element : _elements)
            element->setColor(_model, color);
    }

    void _updateColorImpl(
        const std::string& method,
        const std::vector<ColoringInformation>& variables) final
    {
        if (!variables.empty())
            _colorWithInput(method, variables);
        else
            _colorRandomly(method);
    }

protected:
    virtual std::vector<std::string> _getExtraMethods() const = 0;
    virtual std::vector<std::string> _getValuesForMethod(
        const std::string& method) const = 0;

private:
    void _colorWithInput(const std::string& method,
                         const std::vector<ColoringInformation>& input)
    {
        if (method == METHOD_BY_SECTION)
        {
            const auto updateSectionCB = [&](const std::string& section,
                                             size_t NeuronMaterialMap::*ptr) {
                const auto varIt =
                    std::find_if(input.begin(), input.end(),
                                 [&](const ColoringInformation& ci) {
                                     return ci.variable == section;
                                 });

                if (varIt == input.end())
                    return;

                for (auto& element : _elements)
                {
                    auto& nmm = static_cast<NeuronMaterialMap&>(*element.get());
                    if (nmm.*ptr != std::numeric_limits<size_t>::max())
                        _updateMaterial(nmm.*ptr, (*varIt).color);
                }
            };

            updateSectionCB(brayns::enumToString(NeuronSection::SOMA),
                            &NeuronMaterialMap::soma);
            updateSectionCB(brayns::enumToString(NeuronSection::AXON),
                            &NeuronMaterialMap::axon);
            updateSectionCB(brayns::enumToString(NeuronSection::DENDRITE),
                            &NeuronMaterialMap::dendrite);
            updateSectionCB(brayns::enumToString(
                                NeuronSection::APICAL_DENDRITE),
                            &NeuronMaterialMap::apicalDendrite);
        }
        else
        {
            const auto values = _getValuesForMethod(method);

            std::unordered_map<std::string, std::vector<size_t>> map;
            for (size_t i = 0; i < values.size(); ++i)
                map[values[i]].push_back(i);

            for (const auto& entry : input)
            {
                const auto& color = entry.color;
                auto it = map.find(entry.variable);
                if (it != map.end())
                {
                    for (const auto index : it->second)
                        _elements[index]->setColor(_model, color);
                }
            }
        }
    }

    void _colorRandomly(const std::string& method)
    {
        if (method == METHOD_BY_SECTION)
        {
            const auto updateSectionCB = [&](const brayns::Vector4f& c,
                                             size_t NeuronMaterialMap::*ptr) {
                for (auto& element : _elements)
                {
                    auto& nmm = static_cast<NeuronMaterialMap&>(*element.get());
                    if (nmm.*ptr != std::numeric_limits<size_t>::max())
                        _updateMaterial(nmm.*ptr, c);
                }
            };
            ColorRoulette r;
            updateSectionCB(r.getNextColor(), &NeuronMaterialMap::soma);
            updateSectionCB(r.getNextColor(), &NeuronMaterialMap::axon);
            updateSectionCB(r.getNextColor(), &NeuronMaterialMap::dendrite);
            updateSectionCB(r.getNextColor(),
                            &NeuronMaterialMap::apicalDendrite);
        }
        else
        {
            std::cout << "HERE" << std::endl;
            const auto values = _getValuesForMethod(method);

            ColorDeck deck;
            for (size_t i = 0; i < _elements.size(); ++i)
                _elements[i]->setColor(_model, deck.getColorForKey(values[i]));
        }
    }

protected:
    IDListType _ids;
    std::vector<ElementMaterialMap::Ptr> _elements;
};
