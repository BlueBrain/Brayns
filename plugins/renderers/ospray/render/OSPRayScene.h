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
