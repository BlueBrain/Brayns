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

#include <filesystem>
#include <string>
#include <vector>

namespace
{
class CircuitAccessor
{
public:
    virtual ~CircuitAccessor() = default;
    virtual std::vector<std::string> getLayers(const std::vector<uint64_t> &gids) const = 0;
    virtual std::vector<std::string> getETypes(const std::vector<uint64_t> &gids) const = 0;
    virtual std::vector<std::string> getMTypes(const std::vector<uint64_t> &gids) const = 0;
    virtual std::vector<std::string> getRegions(const std::vector<uint64_t> &gids) const = 0;
    virtual std::vector<std::string> getSynapseClasses(const std::vector<uint64_t> &gids) const = 0;
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
        std::vector<uint64_t> result(_circuit.getNumNeurons());
        std::iota(result.begin(), result.end(), 1u);
        return result;
    }

    std::vector<std::string> getLayers(const std::vector<uint64_t> &gids) const final
    {
        return _getAttrib(gids, brion::NeuronAttributes::NEURON_LAYER);
    }

    std::vector<std::string> getETypes(const std::vector<uint64_t> &gids) const final
    {
        return _getIndexedList(gids, brion::NeuronAttributes::NEURON_ETYPE, brion::NeuronClass::NEURONCLASS_ETYPE);
    }

    std::vector<std::string> getMTypes(const std::vector<uint64_t> &gids) const final
    {
        return _getIndexedList(gids, brion::NeuronAttributes::NEURON_MTYPE, brion::NeuronClass::NEURONCLASS_MTYPE);
    }

    std::vector<std::string> getRegions(const std::vector<uint64_t> &gids) const final
    {
        (void)gids;
        return {};
    }

    std::vector<std::string> getSynapseClasses(const std::vector<uint64_t> &gids) const final
    {
        (void)gids;
        return {};
    }

    std::vector<std::string> getMorphologyNames(const std::vector<uint64_t> &gids) const final
    {
        return _getAttrib(gids, brion::NeuronAttributes::NEURON_MORPHOLOGY_NAME);
    }

private:
    std::vector<std::string> _getIndexedList(
        const std::vector<uint64_t> &gids,
        brion::NeuronAttributes attribute,
        brion::NeuronClass cls) const
    {
        auto indices = _getAttrib(gids, attribute);
        if (indices.empty())
        {
            return {};
        }

        auto allTypes = _circuit.getTypes(cls);

        auto result = std::vector<std::string>();
        result.reserve(gids.size());

        for (size_t i = 0; i < gids.size(); ++i)
        {
            result.push_back(allTypes[std::stoull(indices[i])]);
        }

        return result;
    }

    std::vector<std::string> _getAttrib(const std::vector<uint64_t> &gids, uint32_t attribute) const
    {
        auto gidsSet = brion::GIDSet(gids.begin(), gids.end());
        auto matrix = _readMatrix(gidsSet, attribute);

        if (matrix.empty())
        {
            return {};
        }

        auto idx = matrix.shape()[1] > 1 ? 1ul : 0ul;
        auto data = std::vector<std::string>();
        data.reserve(gidsSet.size());

        for (size_t i = 0; i < gidsSet.size(); ++i)
        {
            data.push_back(matrix[i][idx]);
        }
        return data;
    }

    brion::NeuronMatrix _readMatrix(const brion::GIDSet &gids, uint32_t attribute) const
    {
        try
        {
            return _circuit.get(gids, attribute);
        }
        catch (...)
        {
        }
        return brion::NeuronMatrix();
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
        std::vector<uint64_t> result(_circuit->getNbNeuron());
        std::iota(result.begin(), result.end(), 1u);
        return result;
    }

    std::vector<std::string> getLayers(const std::vector<uint64_t> &gids) const final
    {
        return _tryGetAttribute(gids, [&](auto &range) { return _circuit->getLayers(range); });
    }

    std::vector<std::string> getETypes(const std::vector<uint64_t> &gids) const final
    {
        return _tryGetAttribute(gids, [&](auto &range) { return _circuit->getEtypes(range); });
    }

    std::vector<std::string> getMTypes(const std::vector<uint64_t> &gids) const final
    {
        return _tryGetAttribute(gids, [&](auto &range) { return _circuit->getMtypes(range); });
    }

    std::vector<std::string> getRegions(const std::vector<uint64_t> &gids) const final
    {
        return _tryGetAttribute(gids, [&](auto &range) { return _circuit->getRegions(range); });
    }

    std::vector<std::string> getSynapseClasses(const std::vector<uint64_t> &gids) const final
    {
        return _tryGetAttribute(gids, [&](auto &range) { return _circuit->getSynapseClass(range); });
    }

    std::vector<std::string> getMorphologyNames(const std::vector<uint64_t> &gids) const final
    {
        return _tryGetAttribute(gids, [&](auto &range) { return _circuit->getMorphologies(range); });
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
            auto offset = *it - previousGid;
            sourceIndex += offset;
            assert(sourceIndex < source.size());
            result[i] = source[sourceIndex];
            previousGid = *it;
        }
        return result;
    }

    template<typename Callable>
    std::vector<std::string> _tryGetAttribute(const std::vector<uint64_t> &gids, Callable &&callable) const
    {
        auto range = _getRange(gids);
        try
        {
            return _arrange(callable(range), gids);
        }
        catch (...)
        {
        }

        return {};
    }

    std::unique_ptr<CircuitType> _circuit;
};

class CircuitFactory
{
public:
    static std::unique_ptr<CircuitAccessor> instantiate(const std::string &path, const std::string &population)
    {
        auto lowerCase = brayns::StringCase::toLower(path);
        auto extension = std::filesystem::path(lowerCase).extension().string();

        if (extension == ".mvd2")
        {
            return std::make_unique<MVD2Circuit>(path);
        }

        if (extension == ".mvd3")
        {
            auto mvdCircuit = std::make_unique<MVD3::MVD3File>(path);
            return std::make_unique<GenericCircuit<MVD3::MVD3File>>(std::move(mvdCircuit));
        }

        if (extension == ".h5" || extension == ".hdf5")
        {
            auto mvdCircuit = std::make_unique<MVD::SonataFile>(path, population);
            return std::make_unique<GenericCircuit<MVD::SonataFile>>(std::move(mvdCircuit));
        }

        throw std::runtime_error("Cannot read circuit file from " + path);
    }
};

class MethodQuerier
{
public:
    MethodQuerier(const CircuitAccessor &circuit)
        : _circuit(circuit)
    {
    }

    std::vector<std::string> query(NeuronColorMethod method, const std::vector<uint64_t> &ids)
    {
        switch (method)
        {
        case NeuronColorMethod::ByLayer:
            return _circuit.getLayers(ids);
        case NeuronColorMethod::ByEtype:
            return _circuit.getETypes(ids);
        case NeuronColorMethod::ByMtype:
            return _circuit.getMTypes(ids);
        case NeuronColorMethod::ByMorphology:
            return _circuit.getMorphologyNames(ids);
        case NeuronColorMethod::ByRegion:
            return _circuit.getRegions(ids);
        case NeuronColorMethod::BySynapseClass:
            return _circuit.getSynapseClasses(ids);
        default:
            throw std::invalid_argument("Color method not handled for BlueConfig/CircuitConfig");
        }
    }

private:
    const CircuitAccessor &_circuit;
};

class ValidMethodBuilder
{
public:
    inline static const std::vector<NeuronColorMethod> validMethods = {
        NeuronColorMethod::ByEtype,
        NeuronColorMethod::ByLayer,
        NeuronColorMethod::ByMorphology,
        NeuronColorMethod::ByMtype,
        NeuronColorMethod::ByRegion,
        NeuronColorMethod::BySynapseClass};

    static std::vector<std::string> build(const CircuitAccessor &circuit)
    {
        auto querier = MethodQuerier(circuit);

        auto validMethodNames = std::vector<std::string>();
        validMethodNames.reserve(validMethods.size());

        for (auto method : validMethods)
        {
            auto data = querier.query(method, {1});
            if (data.empty() || data.front().empty())
            {
                continue;
            }
            validMethodNames.push_back(brayns::EnumInfo::getName(method));
        }

        return validMethodNames;
    }
};

} // namespace

namespace bbploader
{
std::vector<std::string> BBPColorMethods::get(const std::string &circuitPath, const std::string &population)
{
    auto circuit = CircuitFactory::instantiate(circuitPath, population);
    return ValidMethodBuilder::build(*circuit);
}

std::vector<std::string> BBPColorValues::get(
    const std::string &circuitPath,
    const std::string &population,
    const std::string &method,
    const std::vector<uint64_t> &ids)
{
    auto methodEnum = brayns::EnumInfo::getValue<NeuronColorMethod>(method);
    auto circuit = CircuitFactory::instantiate(circuitPath, population);
    auto querier = MethodQuerier(*circuit);
    return querier.query(methodEnum, ids);
}

std::vector<std::string>
    BBPColorValues::getAll(const std::string &circuitPath, const std::string &population, const std::string &method)
{
    auto circuit = CircuitFactory::instantiate(circuitPath, population);
    auto ids = circuit->getAllIds();
    return get(circuitPath, population, method, ids);
}
}
