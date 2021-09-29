/* Copyright (c) 2019, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#ifndef TESTRENDERER_H
#define TESTRENDERER_H

#include "TestMaterial.h"

#include <ospray/SDK/common/Material.h>
#include <ospray/SDK/render/Renderer.h>

namespace circuitRenderer
{
class TestRenderer : public ospray::Renderer
{
public:
    TestRenderer();

    std::string toString() const final
    {
        return "circuitRenderer::TestRenderer";
    }

    void commit() override final;

protected:
    std::vector<void*> _lights;
    void** _lightPtr;
    ospray::Data* _lightData;

    TestMaterial* _backgroundMaterial;
};
} // namespace circuitRenderer

#endif
