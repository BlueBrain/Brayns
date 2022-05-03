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

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/common/DataHandler.h>
#include <brayns/engine/geometries/Primitive.h>

#include <api/neuron/NeuronGeometry.h>

#include <ospray/ospray.h>

class MorphologyCircuitComponent final : public brayns::Component
{
public:
    struct MorphologyGeometry
    {
        OSPGeometricModel model{nullptr};
        brayns::Geometry<brayns::Primitive> geometry;
        std::vector<NeuronSectionMapping> sections;
    };

public:
    brayns::Bounds computeBounds(const brayns::Matrix4f &transform) const noexcept override;

    void onCreate() override;

    bool commit() override;

    void onDestroy() override;

    void onInspect(const brayns::InspectContext &context, brayns::JsonObject &writeResult) const noexcept override;

    /**
     * @brief Sets the morphologies of this component
     * @param id
     * @param primitives
     * @param map
     */
    void setMorphologies(
        std::vector<uint64_t> id,
        std::vector<std::vector<brayns::Primitive>> primitives,
        std::vector<std::vector<NeuronSectionMapping>> map) noexcept;

    /**
     * @brief getIDs return the cell IDs in this circuit
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
     * @return std::vector<uint64_t> list of element ids that were not affected
     */
    std::vector<uint64_t> setColorById(const std::map<uint64_t, brayns::Vector4f> &colors) noexcept;

    /**
     * @brief setSectionColor sets colors on a per morphology section basis
     * @param colors pair of neuron section and its corresponding color
     */
    void setColorBySection(const std::vector<std::pair<NeuronSection, brayns::Vector4f>> &sectionColorList) noexcept;

    /**
     * @brief setSimulationColor sets colors based on simulation data. The color buffer is managed by the
     * simulation component.
     * @param color
     * @param mapping per geometry primitive indices into the color buffer
     */
    void setIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &mapping);

    /**
     * @brief Changes the thickness (radii) of the morphology geometries
     *
     * @param multiplier
     */
    void changeThickness(const float multiplier) noexcept;

private:
    std::vector<uint64_t> _ids;
    std::vector<MorphologyGeometry> _morphologies;
    bool _colorsDirty{false};
    bool _geometryDirty{false};
};
