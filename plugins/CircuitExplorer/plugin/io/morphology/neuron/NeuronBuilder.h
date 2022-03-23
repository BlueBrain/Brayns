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

#pragma once

#include <plugin/io/morphology/MorphologyInstance.h>
#include <plugin/io/morphology/neuron/NeuronMorphology.h>

#include <brayns/common/MathTypes.h>

class NeuronInstantiableGeometry
{
public:
    virtual std::unique_ptr<MorphologyInstance> instantiate(const brayns::Vector3f &tr,
                                                            const brayns::Quaternion &rot) const = 0;
};

/**
 * @brief The NeuronBuilder class is the base class to implement
 *        geometry builders that transform the Morphology object into a set
 *        of 3D shapes that are renderable. The result then is instantiated
 *        acording to each cell properties (position and rotation)
 */
class NeuronBuilder
{
public:
    NeuronBuilder(std::string name)
        : _builderName(std::move(name))
    {
    }

    virtual ~NeuronBuilder() = default;

    /**
     * @brief returns this neuron geoemtry builder name, used to identify it
     * @return std::string with the builders name
     */
    const std::string &getName() const noexcept
    {
        return _builderName;
    }

    /**
     * @brief builds the geometry from the given morphology representation. If
     * the builders has already been used to build a geometry, this function has
     * no effect
     */
    virtual std::unique_ptr<NeuronInstantiableGeometry> build(const NeuronMorphology &nm) const = 0;

private:
    const std::string _builderName;
};

/**
 * @brief The NeuronBuilderTable class register and gives access to all
 * available neuron builders, identifying them by their name
 */
class NeuronBuilderTable
{
public:
    /**
     * @brief NeuronBuilderTable constructor registers all available builders
     */
    NeuronBuilderTable();

    /**
     * @brief instantiate a builder by its type and registers it on the table
     */
    template<typename T, typename = std::enable_if_t<std::is_base_of<NeuronBuilder, T>::value>>
    void registerBuilder()
    {
        auto builder = std::make_unique<T>();
        _builders.push_back(std::move(builder));
    }

    /**
     * @brief Search and returns a neuron builder by its name
     *
     * @throws std::runtime_error if the given name does not correspond to an
     * existing builder
     * @returns a reference to a neuron builder
     */
    const NeuronBuilder &getBuilder(const std::string &name) const;

    std::vector<std::string> getAvailableBuilderNames() const noexcept;

private:
    std::vector<std::unique_ptr<NeuronBuilder>> _builders;
};
