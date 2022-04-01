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

#include <brayns/common/MathTypes.h>

#include <io/morphology/neuron/NeuronSection.h>

#include <optional>
#include <set>
#include <string>
#include <vector>

/**
 * @brief The Morphology class is a representation of a morphology file
 */
class NeuronMorphology
{
public:
    struct SectionSample
    {
        brayns::Vector3f position;
        float radius;
    };

    /**
     * @brief The Section class represents a single morphology section, giving access to per-section morphology data
     */
    struct Section
    {
        int32_t id;
        int32_t parentId;
        NeuronSection type;
        std::vector<SectionSample> samples;
    };

    /**
     * @brief The Soma class represents the cell soma body
     */
    struct Soma
    {
        brayns::Vector3f center;
        float radius;
        std::vector<size_t> childrenIndices;
    };

public:
    NeuronMorphology(const std::string &path, const bool soma, const bool axon, const bool dendrites);

    /**
     * @brief returns true if this morphology was loaded with soma
     */
    bool hasSoma() const noexcept;

    /**
     * @brief returns a reference to the soma object.
     * @throws std::runtime_error if the morphology was loaded without soma
     */
    Soma &soma();

    /**
     * @brief returns a const reference to the soma object.
     * @throws std::runtime_error if the morphology was loaded without soma
     */
    const Soma &soma() const;

    /**
     * @brief returns a list of all the sections that were loaded for this morphology
     */
    std::vector<Section> &sections() noexcept;

    /**
     * @brief returns a list of all the sections that were loaded for this morphology
     */
    const std::vector<Section> &sections() const noexcept;

    /**
     * @brief Returns the indices of the children of the given section
     * @param section
     * @return std::vector<size_t>
     */
    std::vector<size_t> sectionChildrenIndices(const Section &section) const noexcept;

private:
    std::optional<Soma> _soma;
    std::vector<Section> _sections;
};
