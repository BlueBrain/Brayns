/* Copyright (c) 2015-2021, EPFL/Blue Brain Project
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

#include <brayns/common/mathTypes.h>

class NeuronInstantiableGeometry
{
public:
    using Ptr = std::unique_ptr<NeuronInstantiableGeometry>;

    virtual MorphologyInstance::Ptr instantiate(
        const brayns::Vector3f& tr, const brayns::Quaternion& rot) const = 0;
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
    using Ptr = std::unique_ptr<NeuronBuilder>;

    template <typename T, typename = std::enable_if_t<
                              std::is_constructible<std::string, T>::value>>
    NeuronBuilder(T&& name)
        : _name(std::forward<T>(name))
    {
    }

    virtual ~NeuronBuilder() = default;

    /**
     * @brief returns this neuron geoemtry builder name, used to identify it
     * @return std::string with the builders name
     */
    const std::string& getName() { return _name; }

    /**
     * @brief builds the geometry from the given morphology representation. If
     * the builders has already been used to build a geometry, this function has
     * no effect
     */
    NeuronInstantiableGeometry::Ptr build(const NeuronMorphology& nm) const
    {
        return _buildImpl(nm);
    }

protected:
    virtual NeuronInstantiableGeometry::Ptr _buildImpl(
        const NeuronMorphology&) const = 0;

private:
    const std::string _name;
};
