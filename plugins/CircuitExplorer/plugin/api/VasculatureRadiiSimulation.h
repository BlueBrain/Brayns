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

#include <brayns/engine/Scene.h>

/**
 * @brief The VasculatureRadiiSimulation controls the update of SONATA
 * Vasculature radii reports.
 *        TODO: Current engines only support color varying simulations. Radii
 * report modifies geometry radii. Add support for generic simulation types on
 * engines refactoring.
 */
class VasculatureRadiiSimulation
{
public:
    /**
     * @brief iterates over all the registered vasculature datasets, gathering
     * the current simulation frame and updating the geometry radii with it.
     */
    static void update(const uint32_t frame, brayns::Scene &scene);
};
