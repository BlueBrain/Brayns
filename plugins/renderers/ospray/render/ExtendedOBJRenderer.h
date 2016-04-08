/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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

struct ExtendedOBJRenderer : public ospray::Renderer
{
    ExtendedOBJRenderer( );
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
    bool lightEmittingMaterialsEnabled;
    bool gradientBackgroundEnabled;
    int randomNumber;
    bool moving;
    float timestamp;
    int spp;
};

} // ::brayns

