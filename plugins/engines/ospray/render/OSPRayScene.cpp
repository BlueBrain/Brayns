/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#include "OSPRayScene.h"
#include "OSPRayRenderer.h"

#include <brayns/common/log.h>
#include <brayns/parameters/SceneParameters.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/volume/VolumeHandler.h>
#include <brayns/io/TextureLoader.h>

namespace brayns
{

const size_t CACHE_VERSION = 5;

struct TextureTypeMaterialAttribute
{
    TextureType type;
    std::string attribute;
};

static TextureTypeMaterialAttribute textureTypeMaterialAttribute[6] =
{
    {TT_DIFFUSE, "map_kd"},
    {TT_NORMALS, "map_Normal"},
    {TT_SPECULAR, "map_ks"},
    {TT_EMISSIVE, "map_a"},
    {TT_OPACITY, "map_d"},
    {TT_REFLECTION, "map_Reflection"}
};

OSPRayScene::OSPRayScene(
    Renderers renderers,
    ParametersManager& parametersManager )
    : Scene( renderers, parametersManager )
    , _ospLightData( 0 )
    , _ospMaterialData( 0 )
    , _ospVolumeData( 0 )
    , _ospSimulationData( 0 )
    , _ospTransferFunctionDiffuseData( 0 )
    , _ospTransferFunctionEmissionData( 0 )
{
}

void OSPRayScene::commit()
{
    for( auto model: _models)
        ospCommit( model.second );
}

OSPModel* OSPRayScene::modelImpl( const size_t timestamp )
{
    if( _models.find( timestamp ) != _models.end())
        return &_models[timestamp];

    int index = -1;
    for( const auto& model: _models )
        if( model.first <= timestamp )
            index = model.first;
    BRAYNS_DEBUG << "Request model for timestamp " << timestamp
                 << ", returned " << index << std::endl;
    return index == -1 ? nullptr : &_models[index];
}


void OSPRayScene::_saveCacheFile()
{
    const std::string& filename = _parametersManager.getGeometryParameters().getSaveCacheFile();
    BRAYNS_INFO << "Saving scene to binary file: " << filename << std::endl;
    std::ofstream file( filename, std::ios::out | std::ios::binary );

    const size_t version = CACHE_VERSION;
    file.write( ( char* )&version, sizeof( size_t ));
    BRAYNS_INFO << "Version: " << version << std::endl;

    const size_t nbModels = _models.size();
    file.write( ( char* )&nbModels, sizeof( size_t ));
    BRAYNS_INFO << nbModels << " models" << std::endl;
    for( const auto& model: _models )
        file.write( ( char* )&model.first, sizeof( size_t ));

    const size_t nbMaterials = _materials.size();
    file.write( ( char* )&nbMaterials, sizeof( size_t ));
    BRAYNS_INFO << nbMaterials << " materials" << std::endl;

    for( size_t materialId = 0; materialId < nbMaterials; ++materialId )
    {
        size_t bufferSize;

        // Spheres
        bufferSize = _timestampSpheresIndices[materialId].size();
        file.write( ( char* )&bufferSize, sizeof( size_t ));
        for( const auto& index: _timestampSpheresIndices[materialId] )
        {
            file.write( ( char* )&index.first, sizeof( size_t ));
            file.write( ( char* )&index.second, sizeof( size_t ));
        }

        bufferSize =
            _serializedSpheresDataSize[materialId] *
            Sphere::getSerializationSize() *
            sizeof( float );
        file.write( ( char* )&bufferSize, sizeof( size_t ));
        file.write( ( char* )_serializedSpheresData[materialId].data(),
            bufferSize );
        if( bufferSize != 0 )
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedSpheresDataSize[materialId]
                         << " Spheres" << std::endl;

        // Cylinders
        bufferSize = _timestampCylindersIndices[materialId].size();
        file.write( ( char* )&bufferSize, sizeof( size_t ));
        for( const auto& index: _timestampCylindersIndices[materialId] )
        {
            file.write( ( char* )&index.first, sizeof( size_t ));
            file.write( ( char* )&index.second, sizeof( size_t ));
        }

        bufferSize =
            _serializedCylindersDataSize[materialId] *
            Cylinder::getSerializationSize() *
            sizeof( float );
        file.write( ( char* )&bufferSize, sizeof( size_t ));
        file.write( ( char* )_serializedCylindersData[materialId].data(),
            bufferSize );
        if( bufferSize != 0 )
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedCylindersDataSize[materialId]
                         << " Cylinders" << std::endl;

        // Cones
        bufferSize = _timestampConesIndices[materialId].size();
        file.write( ( char* )&bufferSize, sizeof( size_t ));
        for( const auto& index: _timestampConesIndices[materialId] )
        {
            file.write( ( char* )&index.first, sizeof( size_t ));
            file.write( ( char* )&index.second, sizeof( size_t ));
        }

        bufferSize =
            _serializedConesDataSize[materialId] *
            Cone::getSerializationSize() *
            sizeof( float );
        file.write( ( char* )&bufferSize, sizeof( size_t ));
        file.write( ( char* )_serializedConesData[materialId].data(),
            bufferSize );
        if( bufferSize != 0 )
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedConesDataSize[materialId]
                         << " Cones" << std::endl;
    }

    file.write( ( char* )&_bounds, sizeof( Boxf ));
    BRAYNS_INFO << _bounds << std::endl;
    file.close();
    BRAYNS_INFO << "Scene successfully saved"<< std::endl;
}

void OSPRayScene::_loadCacheFile()
{
    commitMaterials();

    const std::string& filename = _parametersManager.getGeometryParameters().getLoadCacheFile();
    BRAYNS_INFO << "Loading scene from binary file: " << filename << std::endl;
    std::ifstream file( filename, std::ios::in | std::ios::binary );
    if( !file.good( ))
    {
        BRAYNS_ERROR << "Could not open cache file " << filename << std::endl;
        return;
    }

    size_t version;
    file.read( (char*)&version, sizeof( size_t ));
    BRAYNS_INFO << "Version: " << version << std::endl;

    if( version != CACHE_VERSION )
    {
        BRAYNS_ERROR << "Only version " << CACHE_VERSION
                     << " is supported" << std::endl;
        return;
    }

    _models.clear();
    size_t nbModels;
    file.read( (char*)&nbModels, sizeof( size_t ));
    BRAYNS_INFO << nbModels << " models" << std::endl;
    for( size_t model = 0; model < nbModels; ++model )
    {
        size_t ts;
        file.read( (char*)&ts, sizeof( size_t ));
        BRAYNS_INFO << "Model for ts " << ts << " created" << std::endl;
        _models[ts] = ospNewModel();
    }

    size_t nbMaterials;
    file.read( (char*)&nbMaterials, sizeof( size_t ));
    BRAYNS_INFO << nbMaterials << " materials" << std::endl;
    for( size_t materialId = 0; materialId < nbMaterials; ++materialId )
    {
        // Spheres
        size_t bufferSize = 0;

        file.read( ( char* )&bufferSize, sizeof( size_t ));
        for( size_t i = 0; i<bufferSize; ++i)
        {
            size_t ts;
            file.read( ( char* )&ts, sizeof( size_t ));
            size_t index;
            file.read( ( char* )&index, sizeof( size_t ));
            _timestampSpheresIndices[materialId][ts] = index;
        }

        file.read( ( char* )&bufferSize, sizeof( size_t ));
        _serializedSpheresDataSize[materialId] = bufferSize /
            ( Sphere::getSerializationSize() * sizeof( float ));
        if( bufferSize != 0 )
        {
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedSpheresDataSize[materialId]
                         << " Spheres" << std::endl;
            _serializedSpheresData[materialId].resize( bufferSize );
            file.read( (char*)_serializedSpheresData[materialId].data(),
                bufferSize );
        }

        // Cylinders
        bufferSize = 0;
        file.read( ( char* )&bufferSize, sizeof( size_t ));
        for( size_t i = 0; i<bufferSize; ++i)
        {
            size_t ts;
            file.read( ( char* )&ts, sizeof( size_t ));
            size_t index;
            file.read( ( char* )&index, sizeof( size_t ));
            _timestampCylindersIndices[materialId][ts] = index;
        }

        file.read( (char*)&bufferSize, sizeof( size_t ));
        _serializedCylindersDataSize[materialId] = bufferSize /
            ( Cylinder::getSerializationSize() * sizeof( float ));
        if( bufferSize != 0 )
        {
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedCylindersDataSize[materialId]
                         << " Cylinders" << std::endl;
            _serializedCylindersData[materialId].reserve( bufferSize );
            file.read( (char*)_serializedCylindersData[materialId].data(),
                bufferSize );
        }

        // Cones
        bufferSize = 0;
        file.read( ( char* )&bufferSize, sizeof( size_t ));
        for( size_t i = 0; i<bufferSize; ++i)
        {
            size_t ts;
            file.read( ( char* )&ts, sizeof( size_t ));
            size_t index;
            file.read( ( char* )&index, sizeof( size_t ));
            _timestampConesIndices[materialId][ts] = index;
        }

        file.read( (char*)&bufferSize, sizeof( size_t ));
        _serializedConesDataSize[materialId] = bufferSize /
            ( Cone::getSerializationSize() * sizeof( float ));
        if( bufferSize != 0 )
        {
            BRAYNS_DEBUG << "[" << materialId << "] "
                         << _serializedConesDataSize[materialId]
                         << " Cones" << std::endl;
            _serializedConesData[materialId].reserve( bufferSize );
            file.read( (char*)_serializedConesData[materialId].data(),
                bufferSize );
        }

        _buildParametricOSPGeometry( materialId );
    }

    // Scene bounds
    file.read( ( char* )&_bounds, sizeof( Boxf ));

    BRAYNS_INFO << _bounds << std::endl;
    BRAYNS_INFO << "Scene successfully loaded"<< std::endl;
    file.close();
}

void OSPRayScene::_buildParametricOSPGeometry( const size_t materialId )
{
    // Extended spheres
    for( const auto& timestampSpheresIndex: _timestampSpheresIndices[materialId] )
    {
        const size_t spheresBufferSize =
            timestampSpheresIndex.second * Sphere::getSerializationSize();

        for( const auto& model: _models )
        {
            if( timestampSpheresIndex.first <= model.first )
            {
                OSPGeometry extendedSpheres =
                    ospNewGeometry("extendedspheres");
                OSPData data = ospNewData( spheresBufferSize, OSP_FLOAT,
                    &_serializedSpheresData[materialId][0],
                    OSP_DATA_SHARED_BUFFER );

                ospSetObject(extendedSpheres,
                    "extendedspheres", data );
                ospSet1i(extendedSpheres, "bytes_per_extended_sphere",
                    Sphere::getSerializationSize() * sizeof(float));
                ospSet1i(extendedSpheres,
                    "offset_radius", 3 * sizeof(float));
                ospSet1i(extendedSpheres,
                    "offset_timestamp", 4 * sizeof(float));
                ospSet1i(extendedSpheres,
                    "offset_value", 5 * sizeof(float));

                if(_ospMaterials[materialId])
                    ospSetMaterial( extendedSpheres, _ospMaterials[materialId]);

                ospCommit( extendedSpheres );
                ospAddGeometry( model.second, extendedSpheres );
            }
        }
    }

    // Extended cylinders
    for( const auto& timestampCylindersIndex: _timestampCylindersIndices[materialId] )
    {
        const size_t cylindersBufferSize =
            timestampCylindersIndex.second * Cylinder::getSerializationSize();

        for( const auto& model: _models )
        {
            if( timestampCylindersIndex.first <= model.first )
            {
                OSPGeometry extendedCylinders =
                    ospNewGeometry("extendedcylinders");
                assert( extendedCylinders );

                OSPData data = ospNewData(
                    cylindersBufferSize, OSP_FLOAT,
                    &_serializedCylindersData[materialId][0],
                    OSP_DATA_SHARED_BUFFER );

                ospSetObject( extendedCylinders, "extendedcylinders", data);
                ospSet1i(extendedCylinders, "bytes_per_extended_cylinder",
                    Cylinder::getSerializationSize() * sizeof(float));
                ospSet1i(extendedCylinders,
                    "offset_timestamp", 7 * sizeof(float));
                ospSet1i(extendedCylinders, "offset_value", 8 * sizeof(float));

                if( _ospMaterials[materialId] )
                    ospSetMaterial( extendedCylinders,
                                    _ospMaterials[materialId]);

                ospCommit(extendedCylinders);
                ospAddGeometry( model.second, extendedCylinders);
            }
        }
    }

    // Extended cones
    for( const auto& timestampConesIndex: _timestampConesIndices[materialId] )
    {
        const size_t conesBufferSize =
            timestampConesIndex.second * Cone::getSerializationSize();

        for( const auto& model: _models )
        {
            if( timestampConesIndex.first <= model.first )
            {
                OSPGeometry extendedCones = ospNewGeometry("extendedcones");
                assert(extendedCones);

                OSPData data = ospNewData(
                    conesBufferSize, OSP_FLOAT,
                    &_serializedConesData[materialId][0],
                    OSP_DATA_SHARED_BUFFER );

                ospSetObject(extendedCones, "extendedcones", data);
                ospSet1i(extendedCones, "bytes_per_extended_cone",
                    Cone::getSerializationSize() * sizeof(float));
                ospSet1i(extendedCones, "offset_timestamp", 8 * sizeof(float));
                ospSet1i(extendedCones, "offset_value", 9 * sizeof(float));

                if( _ospMaterials[materialId] )
                    ospSetMaterial( extendedCones, _ospMaterials[materialId]);

                ospCommit( extendedCones );
                ospAddGeometry( model.second, extendedCones );
           }
       }
    }
}

void OSPRayScene::buildGeometry()
{
    // Make sure lights and materials have been initialized before assigning
    // the geometry
    commitMaterials();

    BRAYNS_INFO << "Building OSPRay geometry" << std::endl;

    if( _parametersManager.getGeometryParameters().getGenerateMultipleModels() )
    {
        // Initialize models according to timestamps
        for( size_t materialId = 0; materialId < _materials.size(); ++materialId )
        {
            for( const PrimitivePtr& primitive: _primitives[materialId] )
            {
                const size_t ts = primitive->getTimestamp();
                if( _models.find(ts) == _models.end())
                {
                    _models[ts] = ospNewModel();
                    BRAYNS_INFO << "Model created for timestamp " << ts
                                << ": " << _models[ts] << std::endl;
                }
            }
        }
    }
    if( _models.size() == 0 )
        // If no timestamp is available, create a default model at timestamp 0
        _models[0] = ospNewModel();

    BRAYNS_INFO << "Models to process: " << _models.size() << std::endl;

    size_t totalNbVertices = 0;
    size_t totalNbIndices = 0;

    // Process geometries
    for( size_t materialId = 0; materialId < _materials.size(); ++materialId )
    {
        _serializedSpheresDataSize[materialId] = 0;
        _serializedCylindersDataSize[materialId] = 0;
        _serializedConesDataSize[materialId] = 0;

        size_t sphereCount = 0;
        size_t cylinderCount = 0;
        size_t coneCount = 0;
        if( _primitives.find(materialId) != _primitives.end() )
        {
            for( const PrimitivePtr& primitive: _primitives[materialId] )
            {
                const float ts =
                    ( _models.size() == 1 ) ? 0.f : primitive->getTimestamp();
                switch(primitive->getGeometryType())
                {
                    case GT_SPHERE:
                    {
                        primitive->serializeData(_serializedSpheresData[materialId]);
                        ++_serializedSpheresDataSize[materialId];
                        ++sphereCount;
                        _timestampSpheresIndices[materialId][ts] = sphereCount;
                    }
                    break;
                    case GT_CYLINDER:
                    {
                        primitive->serializeData(_serializedCylindersData[materialId]);
                        ++_serializedCylindersDataSize[materialId];
                        ++cylinderCount;
                        _timestampCylindersIndices[materialId][ts] = cylinderCount;
                    }
                    break;
                    case GT_CONE:
                    {
                        primitive->serializeData(_serializedConesData[materialId]);
                        ++_serializedConesDataSize[materialId];
                        ++coneCount;
                        _timestampConesIndices[materialId][ts] = coneCount;
                    }
                    default:
                        break;
                }
            }

            _buildParametricOSPGeometry( materialId );
        }

        // Triangle mesh
        if( _trianglesMeshes.find(materialId) != _trianglesMeshes.end() )
        {
            OSPGeometry mesh = ospNewGeometry("trianglemesh");
            assert(mesh);
            OSPData vertices = ospNewData(
                _trianglesMeshes[materialId].getVertices().size(),
                OSP_FLOAT3,
                &_trianglesMeshes[materialId].getVertices()[0],
                OSP_DATA_SHARED_BUFFER);
            totalNbVertices += _trianglesMeshes[materialId].getVertices().size();

            OSPData normals = ospNewData(
                _trianglesMeshes[materialId].getNormals().size(),
                OSP_FLOAT3,
                &_trianglesMeshes[materialId].getNormals()[0],
                OSP_DATA_SHARED_BUFFER);
            OSPData indices = ospNewData(
                _trianglesMeshes[materialId].getIndices().size(),
                OSP_INT3,
                &_trianglesMeshes[materialId].getIndices()[0],
                OSP_DATA_SHARED_BUFFER);
            totalNbIndices += _trianglesMeshes[materialId].getIndices().size();

            OSPData colors = ospNewData(
                _trianglesMeshes[materialId].getColors().size(),
                OSP_FLOAT3A,
                &_trianglesMeshes[materialId].getColors()[0],
                OSP_DATA_SHARED_BUFFER);
            OSPData texcoord = ospNewData(
                _trianglesMeshes[materialId].getTextureCoordinates().size(),
                OSP_FLOAT2,
                &_trianglesMeshes[materialId].getTextureCoordinates()[0],
                OSP_DATA_SHARED_BUFFER);
            ospSetObject(mesh,"position",vertices);
            ospSetObject(mesh,"index",indices);
            ospSetObject(mesh,"vertex.normal",normals);
            ospSetObject(mesh,"vertex.color",colors);
            ospSetObject(mesh,"vertex.texcoord",texcoord);
            ospSet1i(mesh, "alpha_type", 0);
            ospSet1i(mesh, "alpha_component", 4);

            if (_ospMaterials[materialId])
                ospSetMaterial(mesh, _ospMaterials[materialId]);

            ospCommit(mesh);

            // Meshes are by default added to all timestamps
            for( const auto& model: _models )
                ospAddGeometry( model.second, mesh);
        }
    }

    commitLights();

    if(!_parametersManager.getGeometryParameters().getLoadCacheFile().empty())
        _loadCacheFile();

    size_t totalNbSpheres = 0;
    size_t totalNbCylinders = 0;
    size_t totalNbCones = 0;
    for( size_t i = 0; i < _materials.size(); ++i )
    {
        totalNbSpheres += _serializedSpheresDataSize[i];
        totalNbCylinders += _serializedCylindersDataSize[i];
        totalNbCones += _serializedConesDataSize[i];
    }

    BRAYNS_INFO << "--------------------" << std::endl;
    BRAYNS_INFO << "Primitive information" << std::endl;
    BRAYNS_INFO << "Spheres  : " << totalNbSpheres << std::endl;
    BRAYNS_INFO << "Cylinders: " << totalNbCylinders << std::endl;
    BRAYNS_INFO << "Cones    : " << totalNbCones << std::endl;
    BRAYNS_INFO << "Vertices : " << totalNbVertices << std::endl;
    BRAYNS_INFO << "Indices  : " << totalNbIndices << std::endl;
    BRAYNS_INFO << "--------------------" << std::endl;

    if(!_parametersManager.getGeometryParameters().getSaveCacheFile().empty())
        _saveCacheFile();
}

void OSPRayScene::commitLights()
{

    for( auto renderer: _renderers )
    {
        OSPRayRenderer* osprayRenderer =
            dynamic_cast< OSPRayRenderer* >( renderer.get( ));

        size_t lightCount = 0;
        for( auto light: _lights )
        {
            DirectionalLight* directionalLight =
                dynamic_cast< DirectionalLight* >( light.get( ));
            if( directionalLight != 0 )
            {
                if( _ospLights.size() <= lightCount )
                    _ospLights.push_back( ospNewLight(
                        osprayRenderer->impl(), "DirectionalLight" ));

                const Vector3f color = directionalLight->getColor( );
                ospSet3f( _ospLights[lightCount], "color",
                    color.x( ), color.y( ), color.z( ));
                const Vector3f direction = directionalLight->getDirection( );
                ospSet3f( _ospLights[lightCount], "direction",
                    direction.x( ), direction.y( ), direction.z( ));
                ospSet1f( _ospLights[lightCount], "intensity",
                    directionalLight->getIntensity( ));
                ospCommit( _ospLights[lightCount] );
            }
            else
            {
                PointLight* pointLight = dynamic_cast< PointLight* >( light.get( ));
                if( pointLight != 0 )
                {
                    if( _ospLights.size() <= lightCount )
                        _ospLights.push_back( ospNewLight(
                            osprayRenderer->impl(), "PointLight" ));

                    const Vector3f position = pointLight->getPosition( );
                    ospSet3f( _ospLights[lightCount], "position",
                        position.x( ), position.y( ), position.z( ));
                    const Vector3f color = pointLight->getColor( );
                    ospSet3f( _ospLights[lightCount], "color",
                        color.x( ), color.y( ), color.z( ));
                    ospSet1f( _ospLights[lightCount], "intensity",
                        pointLight->getIntensity( ));
                    ospSet1f( _ospLights[lightCount], "radius",
                        pointLight->getCutoffDistance( ));
                    ospCommit( _ospLights[lightCount] );
                }
            }
            ++lightCount;
        }

        if( _ospLightData == 0 )
        {
            _ospLightData = ospNewData(
                _lights.size(), OSP_OBJECT,
                &_ospLights[0] );
            ospCommit( _ospLightData );
        }
        ospSetData( osprayRenderer->impl(), "lights", _ospLightData );
    }
}

void OSPRayScene::commitMaterials( const bool updateOnly )
{
    for( const auto& renderer: _renderers )
    {
        OSPRayRenderer* osprayRenderer =
            dynamic_cast<OSPRayRenderer*>( renderer.get( ));
        for( size_t index=0; index < _materials.size(); ++index )
        {
            if( _ospMaterials.size() <= index )
                _ospMaterials.push_back(
                    ospNewMaterial( osprayRenderer->impl(),
                        "ExtendedOBJMaterial" ));

            MaterialPtr material = _materials[index];
            assert(material);

            OSPMaterial& ospMaterial = _ospMaterials[index];
            Vector3f value3f = material->getColor();
            ospSet3f(ospMaterial, "kd", value3f.x(), value3f.y(), value3f.z());
            value3f = material->getSpecularColor();
            ospSet3f(ospMaterial, "ks", value3f.x(), value3f.y(), value3f.z());
            ospSet1f(ospMaterial, "ns", material->getSpecularExponent());
            ospSet1f(ospMaterial, "d", material->getOpacity());
            ospSet1f(ospMaterial, "refraction", material->getRefractionIndex());
            ospSet1f(ospMaterial, "reflection", material->getReflectionIndex());
            ospSet1f(ospMaterial, "a", material->getEmission());

            if( !updateOnly )
            {
                // Textures
                for(auto texture: material->getTextures())
                {
                    TextureLoader textureLoader;
                    if( texture.second != TEXTURE_NAME_SIMULATION )
                        textureLoader.loadTexture(
                            _textures, texture.first, texture.second);

                    OSPTexture2D ospTexture = _createTexture2D(texture.second);
                    ospSetObject(
                        ospMaterial,
                        textureTypeMaterialAttribute[texture.first].attribute.c_str(),
                        ospTexture);

                    BRAYNS_DEBUG << "OSPRay texture assigned to " <<
                        textureTypeMaterialAttribute[texture.first].attribute <<
                        " of material " << index << std::endl;
                }
            }
            ospCommit(ospMaterial);
        }

        if( !updateOnly )
        {
            if( _ospMaterialData == 0 )
            {
                _ospMaterialData = ospNewData(
                    NB_SYSTEM_MATERIALS,
                    OSP_OBJECT,
                    &_ospMaterials[MATERIAL_SYSTEM],
                    OSP_DATA_SHARED_BUFFER );

                ospCommit( _ospMaterialData );
            }
            ospSetData( osprayRenderer->impl(),"materials", _ospMaterialData);
        }

        ospCommit( osprayRenderer->impl() );

    }
}

void OSPRayScene::commitTransferFunctionData()
{
    for( const auto& renderer: _renderers )
    {
        OSPRayRenderer* osprayRenderer = dynamic_cast<OSPRayRenderer*>( renderer.get( ));

        // Transfer function Diffuse colors
        if( !_ospTransferFunctionDiffuseData )
            _ospTransferFunctionDiffuseData = ospNewData(
                _transferFunction.getDiffuseColors().size(), OSP_FLOAT4,
                &_transferFunction.getDiffuseColors()[0], OSP_DATA_SHARED_BUFFER );
        ospCommit( _ospTransferFunctionDiffuseData );
        ospSetData( osprayRenderer->impl(),
            "transferFunctionDiffuseData", _ospTransferFunctionDiffuseData );

        // Transfer function emission data
        _ospTransferFunctionEmissionData = ospNewData(
            _transferFunction.getEmissionIntensities().size(), OSP_FLOAT,
            &_transferFunction.getEmissionIntensities()[0], OSP_DATA_SHARED_BUFFER );
        ospCommit( _ospTransferFunctionEmissionData );
        ospSetData( osprayRenderer->impl(),
            "transferFunctionEmissionData", _ospTransferFunctionEmissionData );

        // Transfer function size
        ospSet1i( osprayRenderer->impl(),
            "transferFunctionSize", _transferFunction.getDiffuseColors().size() );

        // Transfer function range
        ospSet1f( osprayRenderer->impl(),
            "transferFunctionMinValue", _transferFunction.getValuesRange().x() );
        ospSet1f( osprayRenderer->impl(),  "transferFunctionRange",
            _transferFunction.getValuesRange().y() - _transferFunction.getValuesRange().x() );
    }
}

void OSPRayScene::commitVolumeData()
{
    VolumeHandlerPtr volumeHandler = getVolumeHandler();
    if( !volumeHandler )
        return;

    const float timestamp = _parametersManager.getSceneParameters().getTimestamp();
    volumeHandler->setTimestamp( timestamp );
    void* data = volumeHandler->getData();
    if( data )
    {
        for( const auto& renderer: _renderers )
        {
            OSPRayRenderer* osprayRenderer = dynamic_cast<OSPRayRenderer*>( renderer.get( ));

            const size_t size = volumeHandler->getSize();

            _ospVolumeData = ospNewData( size, OSP_UCHAR, data, OSP_DATA_SHARED_BUFFER );
            ospCommit( _ospVolumeData );
            ospSetData( osprayRenderer->impl(), "volumeData", _ospVolumeData );

            const Vector3ui& dimensions = volumeHandler->getDimensions();
            ospSet3i( osprayRenderer->impl(),
                "volumeDimensions", dimensions.x(), dimensions.y(), dimensions.z() );

            const Vector3f& elementSpacing =
                _parametersManager.getVolumeParameters().getElementSpacing();
            ospSet3f( osprayRenderer->impl(),
                "volumeElementSpacing", elementSpacing.x(), elementSpacing.y(), elementSpacing.z());

            const Vector3f& offset = _parametersManager.getVolumeParameters().getOffset();
            ospSet3f( osprayRenderer->impl(),
                "volumeOffset", offset.x(), offset.y(), offset.z() );

            const float epsilon = volumeHandler->getEpsilon(
                elementSpacing, _parametersManager.getVolumeParameters().getSamplesPerRay());
            ospSet1f( osprayRenderer->impl(), "volumeEpsilon", epsilon );
        }
    }
}

void OSPRayScene::commitSimulationData()
{
    if( !_simulationHandler )
        return;

    for( const auto& renderer: _renderers )
    {
        OSPRayRenderer* osprayRenderer = dynamic_cast<OSPRayRenderer*>( renderer.get( ));

        // Simulation data
        const uint64_t frame = _parametersManager.getSceneParameters().getTimestamp();

        _ospSimulationData = ospNewData(
            _simulationHandler->getFrameSize(), OSP_FLOAT,
            _simulationHandler->getFrameData( frame ), OSP_DATA_SHARED_BUFFER );
        ospCommit( _ospSimulationData );
        ospSetData( osprayRenderer->impl(), "simulationData", _ospSimulationData );
        ospCommit(osprayRenderer->impl());
    }
}

OSPTexture2D OSPRayScene::_createTexture2D(const std::string& textureName)
{
    if(_ospTextures.find(textureName) != _ospTextures.end())
        return _ospTextures[textureName];

    Texture2DPtr texture = _textures[textureName];
    if(!texture)
    {
        BRAYNS_WARN << "Texture " << textureName <<
            " is not in the cache" << std::endl;
        return nullptr;
    }

    OSPTextureFormat type = OSP_TEXTURE_R8; // smallest valid type as default
    if(texture->getDepth() == 1)
    {
      if(texture->getNbChannels() == 3) type = OSP_TEXTURE_SRGB;
      if(texture->getNbChannels() == 4) type = OSP_TEXTURE_SRGBA;
    }
    else if(texture->getDepth() == 4)
    {
      if(texture->getNbChannels() == 3) type = OSP_TEXTURE_RGB32F;
      if(texture->getNbChannels() == 4) type = OSP_TEXTURE_RGBA32F;
    }

    BRAYNS_DEBUG << "Creating OSPRay texture from " << textureName << " :" <<
        texture->getWidth() << "x" << texture->getHeight() << "x" <<
        (int)type << std::endl;

    osp::vec2i texSize{texture->getWidth(), texture->getHeight()};
    OSPTexture2D ospTexture = ospNewTexture2D(
        texSize, type, texture->getRawData(), 0);

    assert(ospTexture);
    ospCommit(ospTexture);

    _ospTextures[textureName] = ospTexture;

    return ospTexture;
}

}
