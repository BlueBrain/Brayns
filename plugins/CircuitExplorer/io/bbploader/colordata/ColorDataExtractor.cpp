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

#include "ColorDataExtractor.h"

#include <brayns/utils/string/StringCase.h>

#include <api/neuron/NeuronColorMethod.h>

#include <brain/brain.h>
#include <brion/brion.h>

#include <mvdtool/mvd3.hpp>
#include <mvdtool/sonata.hpp>

#include <string>
#include <vector>

namespace
{
// Custom classes to wrap around the different circuit formats with a common
// interface
class CircuitAccessor
{
public:
    virtual ~CircuitAccessor() = default;
    virtual std::vector<std::string> getLayers(const std::vector<uint64_t> &gids) const = 0;
    virtual std::vector<std::string> getETypes(const std::vector<uint64_t> &gids) const = 0;
    virtual std::vector<std::string> getMTypes(const std::vector<uint64_t> &gids) const = 0;
    virtual std::vector<std::string> getMorphologyNames(const std::vector<uint64_t> &gids) const = 0;

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
        std::vector<uint64_t> result(numNeurons);
        std::iota(result.begin(), result.end(), 1u);
        return result;
    }

    std::vector<std::string> getLayers(const std::vector<uint64_t> &gids) const final
    {
        return _getAttrib(gids, brion::NeuronAttributes::NEURON_LAYER);
    }

    std::vector<std::string> getETypes(const std::vector<uint64_t> &gids) const final
    {
        const auto indices = _getAttrib(gids, brion::NeuronAttributes::NEURON_ETYPE);
        std::vector<std::string> result;

        if (!indices.empty())
        {
            const auto allETypes = _circuit.getTypes(brion::NeuronClass::NEURONCLASS_ETYPE);
            result.resize(gids.size());
            for (size_t i = 0; i < gids.size(); ++i)
            {
                result[i] = allETypes[std::stoull(indices[i])];
            }
        }

        return result;
    }

    std::vector<std::string> getMTypes(const std::vector<uint64_t> &gids) const final
    {
        const auto indices = _getAttrib(gids, brion::NeuronAttributes::NEURON_MTYPE);
        std::vector<std::string> result;

        if (!indices.empty())
        {
            const auto allMTypes = _circuit.getTypes(brion::NeuronClass::NEURONCLASS_MTYPE);
            result.resize(gids.size());
            for (size_t i = 0; i < gids.size(); ++i)
            {
                result[i] = allMTypes[std::stoull(indices[i])];
            }
        }

        return result;
    }

    std::vector<std::string> getMorphologyNames(const std::vector<uint64_t> &gids) const final
    {
        return _getAttrib(gids, brion::NeuronAttributes::NEURON_MORPHOLOGY_NAME);
    }

private:
    std::vector<std::string> _getAttrib(const std::vector<uint64_t> &gids, const uint32_t attrib) const
    {
        const auto gidsSet = brion::GIDSet(gids.begin(), gids.end());
        try
        {
            const auto matrix = _circuit.get(gidsSet, attrib);
            if (matrix.shape()[0] == 0)
            {
                return {};
            }

            const size_t idx = matrix.shape()[1] > 1 ? 1 : 0;
            std::vector<std::string> data(gidsSet.size());
            for (size_t i = 0; i < gidsSet.size(); ++i)
            {
                data[i] = matrix[i][idx];
            }
            return data;
        }
        catch (...)
        {
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
        std::vector<uint64_t> result(numNeurons);
        std::iota(result.begin(), result.end(), 1u);
        return result;
    }

    std::vector<std::string> getLayers(const std::vector<uint64_t> &gids) const final
    {
        std::vector<std::string> result;
        try
        {
            const auto range = _getRange(gids);
            result = _arrange(_circuit->getLayers(range), gids);
        }
        catch (...)
        {
        }

        return result;
    }

    std::vector<std::string> getETypes(const std::vector<uint64_t> &gids) const final
    {
        std::vector<std::string> result;
        try
        {
            const auto range = _getRange(gids);
            result = _arrange(_circuit->getEtypes(range), gids);
        }
        catch (...)
        {
        }

        return result;
    }

    std::vector<std::string> getMTypes(const std::vector<uint64_t> &gids) const final
    {
        std::vector<std::string> result;
        try
        {
            const auto range = _getRange(gids);
            result = _arrange(_circuit->getMtypes(range), gids);
        }
        catch (...)
        {
        }

        return result;
    }

    std::vector<std::string> getMorphologyNames(const std::vector<uint64_t> &gids) const final
    {
        std::vector<std::string> result;
        try
        {
            const auto range = _getRange(gids);
            result = _arrange(_circuit->getMorphologies(range), gids);
        }
        catch (...)
        {
        }

        return result;
    }

private:
    MVD::Range _getRange(const std::vector<uint64_t> &gids) const noexcept
    {
        MVD::Range range;
        range.offset = *gids.begin() - 1; // GIDs start at 1
        range.count = *gids.rbegin() - range.offset;
        return range;
    }

    std::vector<std::string> _arrange(const std::vector<std::string> &source, const std::vector<uint64_t> &gids) const
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
        const auto lowerCasePath = brayns::StringCase::toLower(path);

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
} // namespace

namespace bbploader
{
std::vector<std::string> BBPColorMethods::get(const std::string &circuitPath, const std::string &population)
{
    const auto circuit = CircuitFactory::instantiate(circuitPath, population);

    std::vector<std::string> result;

    const auto layerData = circuit->getLayers({1});
    if (!layerData.empty() && !layerData[0].empty())
    {
        result.push_back(brayns::EnumInfo::getName(NeuronColorMethod::ByLayer));
    }

    const auto mTypeData = circuit->getMTypes({1});
    if (!mTypeData.empty() && !mTypeData[0].empty())
    {
        result.push_back(brayns::EnumInfo::getName(NeuronColorMethod::ByMtype));
    }

    const auto eTypeData = circuit->getETypes({1});
    if (!eTypeData.empty() && !eTypeData[0].empty())
    {
        result.push_back(brayns::EnumInfo::getName(NeuronColorMethod::ByEtype));
    }

    const auto morphData = circuit->getMorphologyNames({1});
    if (!morphData.empty() && !morphData[0].empty())
    {
        result.push_back(brayns::EnumInfo::getName(NeuronColorMethod::ByMorphology));
    }

    return result;
}

std::vector<std::string> BBPColorValues::get(
    const std::string &circuitPath,
    const std::string &population,
    const std::string &method,
    const std::vector<uint64_t> &ids)
{
    const auto methodEnum = brayns::EnumInfo::getValue<NeuronColorMethod>(method);
    const auto circuit = CircuitFactory::instantiate(circuitPath, population);

    std::vector<std::string> result;

    switch (methodEnum)
    {
    case NeuronColorMethod::ByLayer:
    {
        result = circuit->getLayers(ids);
        break;
    }
    case NeuronColorMethod::ByEtype:
    {
        result = circuit->getETypes(ids);
        break;
    }
    case NeuronColorMethod::ByMtype:
    {
        result = circuit->getMTypes(ids);
        break;
    }
    case NeuronColorMethod::ByMorphology:
    {
        result = circuit->getMorphologyNames(ids);
        break;
    }
    default:
    {
        throw std::invalid_argument("Cannot gather information for method '" + method + "'");
    }
    }

    if (!result.empty() && result.size() != ids.size())
    {
        throw std::invalid_argument("Input ids contained duplicated entries");
    }

    return result;
}

std::vector<std::string>
    BBPColorValues::getAll(const std::string &circuitPath, const std::string &population, const std::string &method)
{
    std::vector<uint64_t> ids;
    {
        const auto circuit = CircuitFactory::instantiate(circuitPath, population);
        ids = circuit->getAllIds();
    }

    return get(circuitPath, population, method, ids);
}
}
