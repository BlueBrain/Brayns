/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * Based on OSPRay implementation
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

// ospray
#include <ospray/render/Renderer.h>
#include <ospray/common/Material.h>

// system
#include <vector>

namespace brayns {
struct Camera;
struct Model;

struct SimulationRenderer : public ospray::Renderer
{
    SimulationRenderer( );
    std::string toString( ) const final { return "ospray::OBJRenderer"; }

    std::vector< void* > lightArray;
    std::vector< void* > materialArray;

    Model* world;
    Camera* camera;
    ospray::Data* materialData;
    ospray::Data* lightData;

    void commit( ) final;

    virtual ospray::Material *createMaterial( const char *type );

    bool shadowsEnabled;
    bool softShadowsEnabled;
    float ambientOcclusionStrength;
    bool shadingEnabled;
    bool electronShadingEnabled;
    int randomNumber;
    bool moving;
    float timestamp;
    int spp;
    int simulationNbOffsets;
    int simulationNbFrames;
};

} // ::brayns

