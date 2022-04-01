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

#include "BBPNeuronColorData.h"

#include <brain/brain.h>
#include <brion/brion.h>

#include <mvdtool/mvd3.hpp>
#include <mvdtool/sonata.hpp>

#include <brayns/common/Log.h>
#include <brayns/utils/StringUtils.h>

#include <io/morphology/neuron/NeuronColorMethods.h>
#include <io/morphology/neuron/NeuronSection.h>

namespace bbploader
{
namespace
{
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

    virtual std::vector<uint64_t> getAllIds() const noexcept = 0;
};

class MVD2Circuit final : public CircuitAccessor
{
public:
    MVD2Circuit(const std::string &path)
        : _circuit(path)
    {
    }

    std::vector<uint64_t> getAllIds() const noexcept override
    {
        const auto numNeurons = _circuit.getNumNeurons();
        std::vector<uint64_t> result (numNeurons);
        std::iota(result.begin(), result.end(), 1u);
        return result;
    }

    std::vector<std::string> getLayers(const brain::GIDSet &gids) const final
    {
        const auto result = _getAttrib(gids, brion::NeuronAttributes::NEURON_LAYER);
        if (result.empty())
        {
            brayns::Log::warn("[CE] MVD2Circuit: No layer data found");
        }
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
        {
            result[i] = allETypes[std::stoull(indices[i])];
        }
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
        {
            result[i] = allMTypes[std::stoull(indices[i])];
        }
        return result;
    }

    std::vector<std::string> getMorphologyNames(const brain::GIDSet &gids) const final
    {
        const auto result = _getAttrib(gids, brion::NeuronAttributes::NEURON_MORPHOLOGY_NAME);
        if (result.empty())
        {
            brayns::Log::warn("[CE] MVD2Circuit: No morphology name data found");
        }
        return result;
    }

private:
    std::vector<std::string> _getAttrib(const brain::GIDSet &gids, const uint32_t attrib) const
    {
        try
        {
            const auto matrix = _circuit.get(gids, attrib);
            if (matrix.shape()[0] == 0)
            {
                return {};
            }

            const size_t idx = matrix.shape()[1] > 1 ? 1 : 0;
            std::vector<std::string> data(gids.size());
            for (size_t i = 0; i < gids.size(); ++i)
            {
                data[i] = matrix[i][idx];
            }
            return data;
        }
        catch (const std::exception &)
        {
            // Let the caller inform that it was not possible to retrieve any data
        }

        return {};
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

    std::vector<uint64_t> getAllIds() const noexcept override
    {
        const auto numNeurons = _circuit->getNbNeuron();
        std::vector<uint64_t> result (numNeurons);
        std::iota(result.begin(), result.end(), 1u);
        return result;
    }

    std::vector<std::string> getLayers(const brain::GIDSet &gids) const final
    {
        try
        {
            const auto range = _getRange(gids);
            const auto result = _arrange(_circuit->getLayers(range), gids);
            if (result.empty())
            {
                brayns::Log::warn("[CE] GenericCircuit: No layer data found");
            }
            return result;
        }
        catch (const std::exception &)
        {
            brayns::Log::warn("Unable to retrieve MVD3/Sonata Layers. Variable will not be available for coloring");
        }

        return {};
    }

    std::vector<std::string> getETypes(const brain::GIDSet &gids) const final
    {
        try
        {
            const auto range = _getRange(gids);
            const auto result = _arrange(_circuit->getEtypes(range), gids);
            if (result.empty())
            {
                brayns::Log::warn("[CE] GenericCircuit: No e-type data found");
            }
            return result;
        }
        catch (const std::exception &)
        {
            brayns::Log::warn("Unable to retrieve MVD3/Sonata ETypes. Variable will not be available for coloring");
        }

        return {};
    }

    std::vector<std::string> getMTypes(const brain::GIDSet &gids) const final
    {
        try
        {
            const auto range = _getRange(gids);
            const auto result = _arrange(_circuit->getMtypes(range), gids);
            if (result.empty())
            {
                brayns::Log::warn("[CE] GenericCircuit: No m-type data found");
            }
            return result;
        }
        catch (const std::exception &)
        {
            brayns::Log::warn("Unable to retrieve MVD3/Sonata MTypes. Variable will not be available for coloring");
        }

        return {};
    }

    std::vector<std::string> getMorphologyNames(const brain::GIDSet &gids) const final
    {
        try
        {
            const auto range = _getRange(gids);
            const auto result = _arrange(_circuit->getMorphologies(range), gids);
            if (result.empty())
            {
                brayns::Log::warn("[CE] GenericCircuit: No morphology name data found");
            }
            return result;
        }
        catch (const std::exception &)
        {
            brayns::Log::warn(
                "Unable to retrieve MVD3/Sonata morphology names. Variable will not be available for coloring");
        }

        return {};
    }

private:
    MVD::Range _getRange(const brain::GIDSet &gids) const noexcept
    {
        MVD::Range range;
        range.offset = *gids.begin() - 1; // GIDs start at 1
        range.count = *gids.rbegin() - range.offset;
        return range;
    }

    std::vector<std::string> _arrange(const std::vector<std::string> &source, const brain::GIDSet &gids) const
    {
        if (source.empty())
        {
            return {};
        }
        std::vector<std::string> result(gids.size());
        auto it = gids.begin();
        auto previousGid = *it;
        size_t sourceIndex = 0;
        for (size_t i = 0; i < result.size(); ++i, ++it)
        {
            const auto offset = *it - previousGid;
            sourceIndex += offset;
            if (sourceIndex > source.size())
            {
                throw std::runtime_error("Vector overflowed access");
            }
            result[i] = source[sourceIndex];
            previousGid = *it;
        }
        return result;
    }

    std::unique_ptr<CircuitType> _circuit;
};

// ------------------------------------------------------------------------------------------------

struct CircuitFactory
{
    static std::unique_ptr<CircuitAccessor> instantiate(const std::string &path, const std::string &pop)
    {
        const auto lowerCasePath = brayns::string_utils::toLowercase(path);

        if (lowerCasePath.find(".mvd2") != std::string::npos)
        {
            return std::make_unique<MVD2Circuit>(path);
        }
        else if (lowerCasePath.find(".mvd3") != std::string::npos)
        {
            auto mvdCircuit = std::make_unique<MVD3::MVD3File>(path);
            return std::make_unique<GenericCircuit<MVD3::MVD3File>>(std::move(mvdCircuit));
        }
        else if (lowerCasePath.find(".h5") || lowerCasePath.find(".hdf5"))
        {
            auto mvdCircuit = std::make_unique<MVD::SonataFile>(path, pop);
            return std::make_unique<GenericCircuit<MVD::SonataFile>>(std::move(mvdCircuit));
        }

        return {nullptr};
    }
};

struct CircuitMethods
{
    static std::vector<std::string> get(const CircuitAccessor &circuit)
    {
        std::vector<std::string> result;

        const auto layerData = circuit.getLayers({1});
        if (!layerData.empty() && !layerData[0].empty())
        {
            result.push_back(brayns::enumToString<NeuronColorMethods>(NeuronColorMethods::BY_LAYER));
        }

        const auto mTypeData = circuit.getMTypes({1});
        if (!mTypeData.empty() && !mTypeData[0].empty())
        {
            result.push_back(brayns::enumToString<NeuronColorMethods>(NeuronColorMethods::BY_MTYPE));
        }

        const auto eTypeData = circuit.getETypes({1});
        if (!eTypeData.empty() && !eTypeData[0].empty())
        {
            result.push_back(brayns::enumToString<NeuronColorMethods>(NeuronColorMethods::BY_ETYPE));
        }

        const auto morphData = circuit.getMorphologyNames({1});
        if (!morphData.empty() && !morphData[0].empty())
        {
            result.push_back(brayns::enumToString<NeuronColorMethods>(NeuronColorMethods::BY_MORPHOLOGY));
        }



        return result;
    }
};
} // namespace

BBPNeuronColorData::BBPNeuronColorData(std::string circuitPath, std::string circuitPop)
    : _circuitPath(std::move(circuitPath))
    , _circuitPop(std::move(circuitPop))
{
}

std::vector<std::string> BBPNeuronColorData::getMethods() const noexcept
{
    auto circuitMethods = getCircuitLevelMethods();

    constexpr auto morphologySectionMethod = NeuronColorMethods::BY_MORPHOLOGY_SECTION;
    const auto methodName = brayns::enumToString(morphologySectionMethod);
    circuitMethods.push_back(std::move(methodName));

    return circuitMethods;
}

std::vector<std::string> BBPNeuronColorData::getMethodVariables(const std::string &method) const
{
    const auto methodEnum = brayns::stringToEnum<NeuronColorMethods>(method);
    if(methodEnum == NeuronColorMethods::BY_MORPHOLOGY_SECTION)
    {
        return brayns::enumNames<NeuronSection>();
    }

    return getCircuitLevelMethodVariables(method);
}

std::vector<std::string> BBPNeuronColorData::getMethodValuesForIDs(
        const std::string &method, const std::vector<uint64_t>& ids) const
{
    const auto methodEnum = brayns::stringToEnum<NeuronColorMethods>(method);

    const auto circuit = CircuitFactory::instantiate(_circuitPath, _circuitPop);

    brion::GIDSet gids (ids.begin(), ids.end());

    switch(methodEnum)
    {
    case NeuronColorMethods::BY_LAYER:
    {
        return circuit->getLayers(gids);
    }
    case NeuronColorMethods::BY_ETYPE:
    {
        return circuit->getETypes(gids);
    }
    case NeuronColorMethods::BY_MTYPE:
    {
        return circuit->getMTypes(gids);
    }
    case NeuronColorMethods::BY_MORPHOLOGY:
    {
        return circuit->getMorphologyNames(gids);
    }
    default:
    {
        throw std::invalid_argument("Cannot gather information for method '" + method + "'");
    }
    }

    return {};
}

std::vector<std::string> BBPNeuronColorData::getCircuitLevelMethods() const
{
    const auto circuit = CircuitFactory::instantiate(_circuitPath, _circuitPop);
    return CircuitMethods::get(*circuit);
}

std::vector<std::string> BBPNeuronColorData::getCircuitLevelMethodVariables(const std::string &method) const
{
    auto circuit = CircuitFactory::instantiate(_circuitPath, _circuitPop);
    const auto allIds = circuit->getAllIds();
    circuit.reset(nullptr);

    const auto values = getMethodValuesForIDs(method, allIds);
    const std::set<std::string> unique (values.begin(), values.end());

    return std::vector<std::string>(unique.begin(), unique.end());
}
} // namespace bbploader
