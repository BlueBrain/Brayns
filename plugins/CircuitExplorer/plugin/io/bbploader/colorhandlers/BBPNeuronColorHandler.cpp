/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include "BBPNeuronColorHandler.h"

#include <brain/brain.h>
#include <brion/brion.h>

#include <mvdtool/mvd3.hpp>
#include <mvdtool/sonata.hpp>

#include <brayns/common/Log.h>
#include <brayns/utils/StringUtils.h>

namespace bbploader
{
namespace
{
constexpr char methodByLayer[] = "layer";
constexpr char methodByEType[] = "etype";
constexpr char methodByMtype[] = "mtype";
constexpr char methodByMorph[] = "morphology name";

// Custom classes to wrap around the different circuit formats with a common
// interface
class CircuitAccessor
{
public:
    virtual ~CircuitAccessor() = default;
    virtual std::vector<std::string> getLayers(const brain::GIDSet &gids) const = 0;
    virtual std::vector<std::string> getETypes(const brain::GIDSet &gids) const = 0;
    virtual std::vector<std::string> getMTypes(const brain::GIDSet &gids) const = 0;
    virtual std::vector<std::string> getMorphologyNames(const brain::GIDSet &gids) const = 0;

    std::vector<std::string> getData(const brain::GIDSet &gids, const std::string &method) const
    {
        if (method == methodByLayer)
            return getLayers(gids);
        else if (method == methodByEType)
            return getETypes(gids);
        else if (method == methodByMtype)
            return getMTypes(gids);
        else if (method == methodByMorph)
            return getMorphologyNames(gids);
        return {};
    }
};

class MVD2Circuit : public CircuitAccessor
{
public:
    MVD2Circuit(const std::string &path)
        : _circuit(path)
    {
    }

    std::vector<std::string> getLayers(const brain::GIDSet &gids) const final
    {
        const auto result = _getAttrib(gids, brion::NeuronAttributes::NEURON_LAYER);
        if (result.empty())
            brayns::Log::warn("[CE] MVD2Circuit: No layer data found");
        return result;
    }

    std::vector<std::string> getETypes(const brain::GIDSet &gids) const final
    {
        const auto indices = _getAttrib(gids, brion::NeuronAttributes::NEURON_ETYPE);
        if (indices.empty())
        {
            brayns::Log::warn("[CE] MVD2Circuit: No e-type data found");
            return {};
        }
        const auto allETypes = _circuit.getTypes(brion::NeuronClass::NEURONCLASS_ETYPE);
        std::vector<std::string> result(gids.size());
        for (size_t i = 0; i < gids.size(); ++i)
            result[i] = allETypes[std::stoull(indices[i])];
        return result;
    }

    std::vector<std::string> getMTypes(const brain::GIDSet &gids) const final
    {
        const auto indices = _getAttrib(gids, brion::NeuronAttributes::NEURON_MTYPE);
        if (indices.empty())
        {
            brayns::Log::warn("[CE] MVD2Circuit: No m-type data found");
            return {};
        }
        const auto allMTypes = _circuit.getTypes(brion::NeuronClass::NEURONCLASS_MTYPE);
        std::vector<std::string> result(gids.size());
        for (size_t i = 0; i < gids.size(); ++i)
            result[i] = allMTypes[std::stoull(indices[i])];
        return result;
    }

    std::vector<std::string> getMorphologyNames(const brain::GIDSet &gids) const final
    {
        const auto result = _getAttrib(gids, brion::NeuronAttributes::NEURON_MORPHOLOGY_NAME);
        if (result.empty())
            brayns::Log::warn("[CE] MVD2Circuit: No morphology name data found");
        return result;
    }

private:
    std::vector<std::string> _getAttrib(const brain::GIDSet &gids, const uint32_t attrib) const
    {
        const auto matrix = _circuit.get(gids, attrib);
        if (matrix.shape()[0] == 0)
            return {};

        const size_t idx = matrix.shape()[1] > 1 ? 1 : 0;
        std::vector<std::string> data(gids.size());
        for (size_t i = 0; i < gids.size(); ++i)
            data[i] = matrix[i][idx];
        return data;
    }
    brion::Circuit _circuit;
};

template<class CircuitType>
class GenericCircuit : public CircuitAccessor
{
public:
    GenericCircuit(std::unique_ptr<CircuitType> &&circuit)
        : _circuit(std::move(circuit))
    {
    }

    std::vector<std::string> getLayers(const brain::GIDSet &gids) const final
    {
        const auto range = _getRange(gids);
        const auto result = _arrange(_circuit->getLayers(range), gids);
        if (result.empty())
            brayns::Log::warn("[CE] GenericCircuit: No layer data found");
        return result;
    }

    std::vector<std::string> getETypes(const brain::GIDSet &gids) const final
    {
        const auto range = _getRange(gids);
        const auto result = _arrange(_circuit->getEtypes(range), gids);
        if (result.empty())
            brayns::Log::warn("[CE] GenericCircuit: No e-type data found");
        return result;
    }

    std::vector<std::string> getMTypes(const brain::GIDSet &gids) const final
    {
        const auto range = _getRange(gids);
        const auto result = _arrange(_circuit->getMtypes(range), gids);
        if (result.empty())
            brayns::Log::warn("[CE] GenericCircuit: No m-type data found");
        return result;
    }

    std::vector<std::string> getMorphologyNames(const brain::GIDSet &gids) const final
    {
        const auto range = _getRange(gids);
        const auto result = _arrange(_circuit->getMorphologies(range), gids);
        if (result.empty())
            brayns::Log::warn("[CE] GenericCircuit: No morphology name data found");
        return result;
    }

private:
    MVD::Range _getRange(const brain::GIDSet &gids) const noexcept
    {
        MVD::Range range;
        range.offset = *gids.begin() - 1; // GIDs start at 1
        range.count = *gids.rbegin() - range.offset;
        return range;
    }

    std::vector<std::string> _arrange(const std::vector<std::string> &src, const brain::GIDSet &gids) const
    {
        if (src.empty())
            return {};
        std::vector<std::string> result(gids.size());
        auto it = gids.begin();
        auto prevGID = *it;
        size_t srcIdx = 0;
        for (size_t i = 0; i < result.size(); ++i, ++it)
        {
            const auto offset = *it - prevGID;
            srcIdx += offset;
            if (srcIdx > src.size())
                throw std::runtime_error("Vector overflowed access");
            result[i] = src[srcIdx];
        }
        return result;
    }

    std::unique_ptr<CircuitType> _circuit;
};

// ------------------------------------------------------------------------------------------------

std::unique_ptr<CircuitAccessor> instantiateCircuit(const std::string &path, const std::string &pop)
{
    const auto lowerCasePath = brayns::string_utils::toLowercase(path);
    if (lowerCasePath.find(".mvd2") != std::string::npos)
        return std::make_unique<MVD2Circuit>(path);
    else if (lowerCasePath.find(".mvd3") != std::string::npos)
        return std::make_unique<GenericCircuit<MVD3::MVD3File>>(std::make_unique<MVD3::MVD3File>(path));
    else if (lowerCasePath.find(".h5") || lowerCasePath.find(".hdf5"))
        return std::make_unique<GenericCircuit<MVD::SonataFile>>(std::make_unique<MVD::SonataFile>(path, pop));

    return {nullptr};
}

std::vector<std::string> getAvailableMethods(const CircuitAccessor &circuit)
{
    std::vector<std::string> result;

    const auto layerData = circuit.getLayers({1});
    if (!layerData.empty() && !layerData[0].empty())
        result.push_back(methodByLayer);

    const auto mTypeData = circuit.getMTypes({1});
    if (!mTypeData.empty() && !mTypeData[0].empty())
        result.push_back(methodByMtype);

    const auto eTypeData = circuit.getETypes({1});
    if (!eTypeData.empty() && !eTypeData[0].empty())
        result.push_back(methodByEType);

    const auto morphData = circuit.getMorphologyNames({1});
    if (!morphData.empty() && !morphData[0].empty())
        result.push_back(methodByMorph);

    return result;
}
} // namespace

BBPNeuronColorHandler::BBPNeuronColorHandler(const std::string &circuitPath, const std::string &circuitPop)
    : _circuitPath(circuitPath)
    , _circuitPop(circuitPop)
{
}

std::vector<std::string> BBPNeuronColorHandler::_getExtraMethods() const
{
    const auto circuit = instantiateCircuit(_circuitPath, _circuitPop);
    return getAvailableMethods(*circuit);
}

std::vector<std::string> BBPNeuronColorHandler::_getValuesForMethod(const std::string &method) const
{
    const auto circuit = instantiateCircuit(_circuitPath, _circuitPop);
    std::vector<std::string> data;
    if (method == methodByLayer)
        data = circuit->getLayers(_ids);
    else if (method == methodByEType)
        data = circuit->getETypes(_ids);
    else if (method == methodByMtype)
        data = circuit->getMTypes(_ids);
    else if (method == methodByMorph)
        data = circuit->getMorphologyNames(_ids);

    return data;
}
} // namespace bbploader
