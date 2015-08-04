/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

struct ExtendedOBJRenderer : public ospray::Renderer {
    ExtendedOBJRenderer();
    virtual std::string toString() const { return "ospray::OBJRenderer"; }

    std::vector<void*> lightArray;

    Model        *world;
    Camera       *camera;
    ospray::Data *lightData;

    virtual void commit() final;

    virtual ospray::Material *createMaterial(const char *type) final;

    bool shadowsEnabled;
    bool softShadowsEnabled;
    bool ambientOcclusionEnabled;
    bool shadingEnabled;
    bool electronShadingEnabled;
    bool lightEmittingMaterialsEnabled;
    bool gradientBackgroundEnabled;
    int  randomNumber;
    bool moving;
    int  frameNumber;
    int  spp;
};

} // ::brayns

