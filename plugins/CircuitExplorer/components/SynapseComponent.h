/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include <brayns/engine/geometry/GeometryView.h>
#include <brayns/engine/model/ModelComponents.h>

/**
 * @brief The SynapseComponent class is a synapse geometry rendering component in which the synapse geometries are
 * grouped by the cell to which they are connected to (not the one they are born from)
 */
class SynapseComponent final : public brayns::Component
{
public:
    SynapseComponent(std::map<uint64_t, std::vector<brayns::Sphere>> &synapses);

    brayns::Bounds computeBounds(const brayns::Matrix4f &transform) const noexcept override;
    bool commit() override;
    void onCreate() override;

    /**
     * @brief getCellIds Return the cell ids by which the synapses are grouped
     * @return const std::vector<uint64_t> &
     */
    const std::vector<uint64_t> &getCellIds() const noexcept;

    /**
     * @brief setColor Sets all the synapses to the same specified color
     * @param color
     */
    void setColor(const brayns::Vector4f &color) noexcept;

    /**
     * @brief setColorById Sets the synapse color on a per associated-cell-id basis. The input vector must contain
     * an element for each cell ID in this component
     * @param colors
     */
    void setColorById(const std::vector<brayns::Vector4f> &colors);

    /**
     * @brief setColorById Sets the synapse color on ap er associated-cell-id basis.
     * @param colorMap
     * @return std::vector<uint64_t> list of ids that were not affected
     */
    std::vector<uint64_t> setColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap);

    /**
     * @brief setSimulationColor sets colors based on simulation data. The color buffer is managed by the
     * simulation component.
     * @param color
     * @param mapping per geometry primitive indices into the color buffer
     */
    void setIndexedColor(const std::vector<brayns::Vector4f> &color, const std::vector<uint8_t> &mapping);

private:
    std::vector<uint64_t> _cellIds;
    std::vector<brayns::Geometry> _geometries;
    std::vector<brayns::GeometryView> _views;
    bool _colorsDirty{false};
};
