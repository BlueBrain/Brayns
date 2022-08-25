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
#include <brayns/engine/geometry/types/Capsule.h>
#include <brayns/engine/model/ModelComponents.h>

#include <api/vasculature/VasculatureSection.h>

class VasculatureComponent final : public brayns::Component
{
public:
    VasculatureComponent(
        std::vector<uint64_t> ids,
        std::vector<brayns::Capsule> geometry,
        std::vector<VasculatureSection> sections);

    brayns::Bounds computeBounds(const brayns::Matrix4f &transform) const noexcept override;

    void onCreate() override;

    bool commit() override;

    /**
     * @brief getIDs return the ids of the vessels in this vasculature
     * @return
     */
    const std::vector<uint64_t> &getIDs() const noexcept;

    /**
     * @brief setColor sets an unique color for all the cells
     * @param color
     */
    void setColor(const brayns::Vector4f &color) noexcept;

    /**
     * @brief setColorBySection sets the color on a per-vasculature-section basis
     * @param colormap
     */
    void setColorBySection(const std::vector<std::pair<VasculatureSection, brayns::Vector4f>> &colormap) noexcept;

    /**
     * @brief setColorById sets colors on a per id (per cell) basis, with the argument being a vector with
     * a color for each ID
     * @param colors
     */
    void setColorById(std::vector<brayns::Vector4f> colors) noexcept;

    /**
     * @brief setColorById sets colors on a per-id (per cell) basis
     * @param colors map of id and its corresponding color
     * @param std::vector<uint64_t> list of vasculature node ids that were not affected
     */
    std::vector<uint64_t> setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept;

    /**
     * @brief setSimulationColor sets colors based on simulation data. The color buffer is managed by the
     * simulation component.
     * @param color
     * @param mapping per geometry primitive indices into the color buffer
     */
    void setSimulationColor(const std::vector<brayns::Vector4f> &color, const std::vector<uint8_t> &mapping) noexcept;

    /**
     * @brief Get the Geometry object
     *
     * @return brayns::Geometry
     */
    brayns::Geometry &getGeometry() noexcept;

private:
    std::vector<uint64_t> _ids;
    brayns::Geometry _geometry;
    brayns::GeometryView _view;
    std::vector<VasculatureSection> _sections;
    std::vector<brayns::Vector4f> _colors;
};
