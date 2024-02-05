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

#include <plugin/io/morphology/neuron/NeuronSection.h>

#include <brayns/common/MathTypes.h>

#include <set>
#include <string>
#include <vector>

/**
 * @brief The Morphology class is a representation of a morphology file into a
 * set of structures that aids on the conversion into a 3D shape
 */
class NeuronMorphology
{
public:
    /**
     * @brief The Section class represents a single morphology section,
     *        giving easy access to per-section morphology data
     */
    class Section
    {
    public:
        Section(const int32_t id, const int32_t parentId,
                const NeuronSection type);

        const int32_t id;
        const int32_t parentId;
        const NeuronSection type;
        std::vector<brayns::Vector4f> samples;
    };

    /**
     * @brief The Soma class represents the cell soma body
     */
    class Soma
    {
    public:
        Soma(const brayns::Vector3f& center, const float radius);

        const brayns::Vector3f center;
        float radius;
        std::vector<Section*> children;
    };

public:
    NeuronMorphology(const std::string& path, const bool soma, const bool axon,
                     const bool dendrites);

    /**
     * @brief returns true if this morphology was loaded with soma
     */
    bool hasSoma() const noexcept;

    /**
     * @brief returns a reference to the soma object.
     * @throws std::runtime_error if the morphology was loaded without soma
     */
    Soma& soma();
    const Soma& soma() const;

    /**
     * @brief returns a list of all the sections that were loaded for this
     * morphology
     */
    std::vector<Section>& sections() noexcept;
    const std::vector<Section>& sections() const noexcept;

    /**
     * @brief returns a list with pointers to all the sections which are
     * children of the given section
     */
    std::vector<Section*> sectionChildren(const Section& section) noexcept;
    std::vector<const Section*> sectionChildren(const Section& section) const
        noexcept;

    /**
     * @brief Returns the parent section of the passed section. If the passed
     * section is a root section, or if the morphology is incomplete and no
     * parent is found, it will return null
     */
    Section* parent(const Section& section) noexcept;

    /**
     * @brief Returns the parent section of the passed section. If the passed
     * section is a root section, or if the morphology is incomplete and no
     * parent is found, it will return null
     */
    const Section* parent(const Section& section) const noexcept;

private:
    NeuronMorphology() = default;

    const std::string _morphologyPath;
    std::unique_ptr<Soma> _soma;
    std::vector<Section> _sections;
};
