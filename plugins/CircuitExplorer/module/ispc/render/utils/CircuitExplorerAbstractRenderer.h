/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of the circuit explorer for Brayns
 * <https://github.com/favreau/Brayns-UC-CircuitExplorer>
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

#ifndef CircuitExplorerAbstractRenderer_H
#define CircuitExplorerAbstractRenderer_H

// obj
#include "../CircuitExplorerMaterial.h"

// ospray
#include <ospray/SDK/common/Material.h>
#include <ospray/SDK/render/Renderer.h>

// system
#include <vector>

namespace circuitExplorer
{
/**
 * The CircuitExplorerAbstractRenderer class implements a base renderer for all
 * Brayns custom implementations
 */
class CircuitExplorerAbstractRenderer : public ospray::Renderer
{
public:
    void commit() override;

protected:
    std::vector<void*> _lightArray;
    void** _lightPtr;
    ospray::Data* _lightData;

    brayns::obj::CircuitExplorerMaterial* _bgMaterial;

    float _timestamp{0.f};
    ospray::uint32 _maxBounces{10};
    float _exposure{1.f};
};
} // namespace circuitExplorer

#endif // CircuitExplorerAbstractRenderer_H
