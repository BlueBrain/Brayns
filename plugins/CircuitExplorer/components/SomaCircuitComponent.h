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
#include <brayns/engine/geometry/types/Sphere.h>
#include <brayns/engine/model/ModelComponents.h>

#include <api/neuron/NeuronSection.h>

class SomaCircuitComponent final : public brayns::Component
{
public:
    SomaCircuitComponent(std::vector<uint64_t> ids, std::vector<brayns::Sphere> geometry);

    brayns::Bounds computeBounds(const brayns::Matrix4f &transform) const noexcept override;

    bool commit() override;

    void onCreate() override;

    void onInspect(const brayns::InspectContext &context, brayns::JsonObject &writeResult) const noexcept override;

    /**
     * @brief getIDs return the ID of the cells in this circuit
     * @return const std::vector<uint64_t> &
     */
    const std::vector<uint64_t> &getIDs() const noexcept;

    /**
     * @brief setColor sets an unique color for all the cells
     * @param color
     */
    void setColor(const brayns::Vector4f &color) noexcept;

    /**
     * @brief setColorById sets colors on a per id (per cell) basis, with the argument being a vector with
     * a color for each ID
     * @param colors
     */
    void setColorById(const std::vector<brayns::Vector4f> &colors);

    /**
     * @brief setColorById sets colors on a per-id (per cell) basis
     * @param colors map of id and its corresponding color
     * @return std::vector<uint64_t> list of soma ids that were not affected
     */
    std::vector<uint64_t> setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept;

    /**
     * @brief setSimulationColor sets colors based on simulation data. The color buffer is managed by the
     * simulation component.
     * @param color
     * @param mapping per geometry primitive indices into the color buffer
     */
    void setIndexedColor(const std::vector<brayns::Vector4f> &colors, const std::vector<uint8_t> &mapping);

private:
    std::vector<uint64_t> _ids;
    brayns::Geometry _geometry;
    brayns::GeometryView _view;
    std::vector<brayns::Vector4f> _colors;
};
