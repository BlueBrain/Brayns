/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef OSPRAYSCENE_H
#define OSPRAYSCENE_H

#include <brayns/common/types.h>
#include <brayns/common/scene/Scene.h>
#include <ospray/ospray.h>
#include <ospray/common/OSPCommon.h>
#include <fstream>

namespace brayns
{

class OSPRayScene: public brayns::Scene
{
public:
    OSPRayScene( RendererPtr renderer, GeometryParameters& geometryParameters );

    void commit() final;
    void loadData() final;
    void buildGeometry() final;

    OSPModel impl() {return _scene;}

private:
    void _commitLights();
    void _commitMaterials();

    OSPTexture2D _createTexture2D(const std::string& textureName);

    void _buildParametricOSPGeometry( const size_t materialId );
    void _loadCacheFile();
    void _saveCacheFile();

    OSPModel _scene;
    std::map<size_t, OSPMaterial> _ospMaterials;
    std::map<std::string, OSPTexture2D> _ospTextures;
    OSPData _ospLightData;

    std::map<size_t, floats> _serializedSpheresData;
    std::map<size_t, floats> _serializedCylindersData;
    std::map<size_t, size_t> _serializedSpheresDataSize;
    std::map<size_t, size_t> _serializedCylindersDataSize;
};

}
#endif // OSPRAYSCENE_H
