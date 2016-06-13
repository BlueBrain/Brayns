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
    OSPRayScene(
        RendererMap renderer,
        SceneParameters& sceneParameters,
        GeometryParameters& geometryParameters );

    void commit() final;
    void buildGeometry() final;
    void commitLights() final;
    void commitMaterials( const bool updateOnly = false ) final;

    OSPModel* modelImpl( const size_t timestamp );

private:

    OSPTexture2D _createTexture2D(const std::string& textureName);

    void _buildParametricOSPGeometry( const size_t materialId );
    void _loadCacheFile();
    void _saveCacheFile();

    std::map< size_t, OSPModel > _models;
    std::vector<OSPMaterial> _ospMaterials;
    std::map<std::string, OSPTexture2D> _ospTextures;

    std::vector< OSPLight > _ospLights;
    OSPData _ospLightData;
    OSPData _ospMaterialData;

    std::map< float, size_t > _timestamps;

    std::map<size_t, floats> _serializedSpheresData;
    std::map<size_t, floats> _serializedCylindersData;
    std::map<size_t, floats> _serializedConesData;
    std::map<size_t, size_t> _serializedSpheresDataSize;
    std::map<size_t, size_t> _serializedCylindersDataSize;
    std::map<size_t, size_t> _serializedConesDataSize;

    std::map< size_t, std::map< size_t, size_t > > _timestampSpheresIndices;
    std::map< size_t, std::map< size_t, size_t > > _timestampCylindersIndices;
    std::map< size_t, std::map< size_t, size_t > > _timestampConesIndices;
};

}
#endif // OSPRAYSCENE_H
