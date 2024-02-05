/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "utils/CircuitExplorerSimulationRenderer.h"

namespace circuitExplorer
{
/**
 * @brief The CircuitExplorerBasicRenderer class can perform fast transparency
 * and mapping of simulation data on the geometry
 */
class CircuitExplorerBasicRenderer : public CircuitExplorerSimulationRenderer
{
public:
    CircuitExplorerBasicRenderer();

    /**
       Returns the class name as a string
       @return string containing the full name of the class
    */
    std::string toString() const final
    {
        return "CircuitExplorerBasicRenderer";
    }
    void commit() final;

private:
    float _simulationThreshold{0.f};
};

} // namespace circuitExplorer
