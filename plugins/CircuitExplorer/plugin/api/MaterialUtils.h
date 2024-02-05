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

#include <cstdint>

#include <brayns/engine/Model.h>

#include <plugin/api/MaterialEnums.h>

/** Additional marterial attributes key names */
// TODO: Remove those once engines have been refactored
constexpr char MATERIAL_PROPERTY_CAST_USER_DATA[] = "cast_simulation_data";
constexpr char MATERIAL_PROPERTY_SHADING_MODE[] = "shading_mode";
constexpr char MATERIAL_PROPERTY_CLIPPING_MODE[] = "clipping_mode";
constexpr char MATERIAL_PROPERTY_USER_PARAMETER[] = "user_parameter";

/**
 * @brief The CircuitExplorerMaterial class holds utility functionality to
 * create and manitpulate CiruitExplorer materials.
 *
 *        TODO: Remove with engine refactoring
 */
class CircuitExplorerMaterial
{
public:
    /**
     * @brief creates a material in the given model, initializing the extra
     * CircuitExplorerMaterial attributes so that it can be rendered
     * appropiately
     */
    static size_t create(brayns::Model& model,
                         const brayns::Vector3f& color = brayns::Vector3f(1.f,
                                                                          1.f,
                                                                          1.f),
                         const float opacity = 1.f);

    /**
     * @brief creates an unlit material in the given model, initializing the
     * extra CircuitExplorerMaterial attributes so that it can be rendered
     * appropiately
     */
    static size_t createUnlit(
        brayns::Model& model,
        const brayns::Vector3f& color = brayns::Vector3f(1.f, 1.f, 1.f),
        const float opacity = 1.f);

    /**
     * @brief adds the CircuitExplorer additional material attributes to all the
     * materials of a given model
     */
    static void addExtraAttributes(brayns::Model& model);

    /**
     * @brief manipulates the 'cast_simulation_data' parameter of
     * CircuitExplorerMaterial, allowing to show simulation colors (true) or
     * geometry color (false) on demand.
     */
    static void setSimulationColorEnabled(brayns::Model& model,
                                          const bool value);

private:
    /**
     * @brief Returns a PropertyMap with CircuitExplorer's custom material
     * attributes
     */
    static const brayns::PropertyMap& _getExtraAttributes() noexcept;
};
