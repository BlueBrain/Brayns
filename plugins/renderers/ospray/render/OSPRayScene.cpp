/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "OSPRayScene.h"
#include "OSPRayRenderer.h"

#include <brayns/common/log.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/io/TextureLoader.h>

namespace brayns
{

const size_t CACHE_VERSION_1 = 1;
const size_t CACHE_VERSION_2 = 2;
const size_t CACHE_VERSION_3 = 3;

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
    RendererMap renderers,
    GeometryParameters& geometryParameters )
    : Scene( renderers, geometryParameters ),
    _scene( ospNewModel( ))
{
}

void OSPRayScene::commit()
{
    ospCommit(_scene);
}

void OSPRayScene::_saveCacheFile()
{
    const std::string& filename = _geometryParameters.getSaveCacheFile();
    BRAYNS_INFO << "Saving scene to binary file: " << filename << std::endl;
    std::ofstream file( filename, std::ios::out | std::ios::binary );

    const size_t version = CACHE_VERSION_3;
    file.write( ( char* )&version, sizeof( size_t ));

    const size_t nbMaterials = _materials.size();
    file.write( ( char* )&nbMaterials, sizeof( size_t ));

    for( size_t materialId = 0; materialId < nbMaterials; ++materialId )
    {
        size_t bufferSize =
            _serializedSpheresDataSize[materialId] *
            Sphere::getSerializationSize() *
            sizeof( float );
        file.write( ( char* )&bufferSize, sizeof( size_t ));
        file.write( ( char* )_serializedSpheresData[materialId].data(),
            bufferSize );

        bufferSize =
            _serializedCylindersDataSize[materialId] *
            Cylinder::getSerializationSize() *
            sizeof( float );
        file.write( ( char* )&bufferSize, sizeof( size_t ));
        file.write( ( char* )_serializedCylindersData[materialId].data(),
            bufferSize );

        bufferSize =
            _serializedConesDataSize[materialId] *
            Cone::getSerializationSize() *
            sizeof( float );
        file.write( ( char* )&bufferSize, sizeof( size_t ));
        file.write( ( char* )_serializedConesData[materialId].data(),
            bufferSize );
    }

    file.write( ( char* )&_bounds, sizeof( Boxf ));

    Texture2DPtr simulationTexture = _textures[TEXTURE_NAME_SIMULATION];
    if( simulationTexture )
    {
        const size_t width = simulationTexture->getWidth();
        const size_t height = simulationTexture->getHeight();
        const size_t nbChannels = simulationTexture->getNbChannels();
        size_t simulationTextureSize = width * height * nbChannels;
        file.write( ( char* )&width, sizeof( size_t ));
        file.write( ( char* )&height, sizeof( size_t ));
        file.write( ( char* )&nbChannels, sizeof( size_t ));
        file.write( ( char* )simulationTexture->getRawData(), sizeof(char) * simulationTextureSize );
    }
    file.close();
}

void OSPRayScene::_loadCacheFile()
{
    _commitMaterials();

    const std::string& filename = _geometryParameters.getLoadCacheFile();
    BRAYNS_INFO << "Loading scene from binary file: " << filename << std::endl;
    std::ifstream file( filename, std::ios::in | std::ios::binary );

    size_t version;
    file.read( (char*)&version, sizeof( size_t ));
    if( version == CACHE_VERSION_3 )
    {
        size_t nbMaterials;
        file.read( (char*)&nbMaterials, sizeof( size_t ));
        for( size_t materialId = 0; materialId < nbMaterials; ++materialId )
        {
            size_t bufferSize = 0;
            file.read( ( char* )&bufferSize, sizeof( size_t ));
            _serializedSpheresDataSize[materialId] = bufferSize /
                ( Sphere::getSerializationSize() * sizeof( float ));
            if( bufferSize != 0 )
            {
                _serializedSpheresData[materialId].resize( bufferSize );
                file.read( (char*)_serializedSpheresData[materialId].data(),
                    bufferSize );
            }

            bufferSize = 0;
            file.read( (char*)&bufferSize, sizeof( size_t ));
            _serializedCylindersDataSize[materialId] = bufferSize /
                ( Cylinder::getSerializationSize() * sizeof( float ));
            if( bufferSize != 0 )
            {
                _serializedCylindersData[materialId].reserve( bufferSize );
                file.read( (char*)_serializedCylindersData[materialId].data(),
                    bufferSize );
            }

            bufferSize = 0;
            file.read( (char*)&bufferSize, sizeof( size_t ));
            _serializedConesDataSize[materialId] = bufferSize /
                ( Cone::getSerializationSize() * sizeof( float ));
            if( bufferSize != 0 )
            {
                _serializedConesData[materialId].reserve( bufferSize );
                file.read( (char*)_serializedConesData[materialId].data(),
                    bufferSize );
            }

            _buildParametricOSPGeometry( materialId );
        }

        // Scene bounds
        file.read( ( char* )&_bounds, sizeof( Boxf ));

        // Read simulation texture
        size_t width;
        file.read( (char*)&width, sizeof( size_t ));
        if( !file.eof() )
        {
            size_t height;
            file.read( (char*)&height, sizeof( size_t ));
            size_t nbChannels;
            file.read( (char*)&nbChannels, sizeof( size_t ));
            size_t simulationTextureSize = width * height * nbChannels;

            BRAYNS_INFO << "Loading simulation texture: " <<
                width << "x" << height << "x" << nbChannels << std::endl;

            uint8_ts bytes;
            bytes.resize( simulationTextureSize );
            file.read( (char*)bytes.data(), simulationTextureSize );

            Texture2DPtr simulationTexture( new Texture2D );
            simulationTexture->setWidth(width);
            simulationTexture->setHeight(height);
            simulationTexture->setNbChannels(nbChannels);
            simulationTexture->setDepth(1);
            simulationTexture->setRawData( bytes.data(), simulationTextureSize );

            _textures[TEXTURE_NAME_SIMULATION] = simulationTexture;

            _materials[MATERIAL_SIMULATION]->setTexture(
                TT_DIFFUSE, TEXTURE_NAME_SIMULATION );
            _commitMaterials();
        }
        BRAYNS_INFO << _bounds << std::endl;
    }
    else
    {
        BRAYNS_ERROR << "Only version " << CACHE_VERSION_3 <<
            " is currently supported" << std::endl;
    }
    file.close();
}

void OSPRayScene::_buildParametricOSPGeometry( const size_t materialId )
{
    OSPGeometry* extendedSpheres = new OSPGeometry[_materials.size()];
    OSPGeometry* extendedCylinders = new OSPGeometry[_materials.size()];
    OSPGeometry* extendedCones = new OSPGeometry[_materials.size()];

    // Extended spheres
    const size_t spheresBufferSize =
        _serializedSpheresDataSize[materialId] * Sphere::getSerializationSize();
    extendedSpheres[materialId] = ospNewGeometry("extendedspheres");
    OSPData data = ospNewData(
        spheresBufferSize, OSP_FLOAT,
        &_serializedSpheresData[materialId][0], OSP_DATA_SHARED_BUFFER );

    ospSetObject(extendedSpheres[materialId],
        "extendedspheres", data);
    ospSet1i(extendedSpheres[materialId], "bytes_per_extended_sphere",
        Sphere::getSerializationSize() * sizeof(float));
    ospSet1i(extendedSpheres[materialId],
        "offset_radius", 3 * sizeof(float));
    ospSet1i(extendedSpheres[materialId],
        "offset_timestamp", 4 * sizeof(float));
    ospSet1i(extendedSpheres[materialId],
        "offset_value", 5 * sizeof(float));

    if(_ospMaterials[materialId])
        ospSetMaterial(extendedSpheres[materialId], _ospMaterials[materialId]);

    ospCommit(extendedSpheres[materialId]);
    ospAddGeometry(_scene, extendedSpheres[materialId]);

    // Extended cylinders
    const size_t cylindersBufferSize =
        _serializedCylindersDataSize[materialId] *
        Cylinder::getSerializationSize();
    extendedCylinders[materialId] = ospNewGeometry("extendedcylinders");
    assert(extendedCylinders[materialId]);

    data = ospNewData(
        cylindersBufferSize, OSP_FLOAT,
        &_serializedCylindersData[materialId][0], OSP_DATA_SHARED_BUFFER );

    ospSetObject(extendedCylinders[materialId],
        "extendedcylinders", data);
    ospSet1i(extendedCylinders[materialId], "bytes_per_extended_cylinder",
        Cylinder::getSerializationSize() * sizeof(float));
    ospSet1i(extendedCylinders[materialId],
        "offset_timestamp", 7 * sizeof(float));
    ospSet1i(extendedCylinders[materialId],
        "offset_value", 8 * sizeof(float));

    if(_ospMaterials[materialId])
        ospSetMaterial(extendedCylinders[materialId], _ospMaterials[materialId]);

    ospCommit(extendedCylinders[materialId]);
    ospAddGeometry(_scene, extendedCylinders[materialId]);

    // Extended cones
    const size_t conesBufferSize =
        _serializedConesDataSize[materialId] * Cone::getSerializationSize();
    extendedCones[materialId] = ospNewGeometry("extendedcones");
    assert(extendedCones[materialId]);

    data = ospNewData(
        conesBufferSize, OSP_FLOAT,
        &_serializedConesData[materialId][0], OSP_DATA_SHARED_BUFFER );

    ospSetObject(extendedCones[materialId],
        "extendedcones", data);
    ospSet1i(extendedCylinders[materialId], "bytes_per_extended_cone",
        Cone::getSerializationSize() * sizeof(float));
    ospSet1i(extendedCones[materialId],
        "offset_timestamp", 8 * sizeof(float));
    ospSet1i(extendedCones[materialId],
        "offset_value", 9 * sizeof(float));

    if(_ospMaterials[materialId])
        ospSetMaterial(extendedCones[materialId], _ospMaterials[materialId]);

    ospCommit(extendedCones[materialId]);
    ospAddGeometry(_scene, extendedCones[materialId]);

    delete extendedCones;
    delete extendedSpheres;
    delete extendedCylinders;
}

void OSPRayScene::buildGeometry()
{
    // Make sure lights and materials have been initialized before assigning
    // the geometry
    _commitMaterials();

    BRAYNS_INFO << "Building OSPRay geometry" << std::endl;

    for( size_t materialId=0; materialId<_materials.size(); ++materialId )
    {
        _serializedSpheresDataSize[materialId] = 0;
        _serializedCylindersDataSize[materialId] = 0;
        for( const PrimitivePtr& primitive: _primitives[materialId] )
        {
            switch(primitive->getGeometryType())
            {
                case GT_SPHERE:
                {
                    primitive->serializeData(_serializedSpheresData[materialId]);
                    ++_serializedSpheresDataSize[materialId];
                }
                break;
                case GT_CYLINDER:
                {
                    primitive->serializeData(_serializedCylindersData[materialId]);
                    ++_serializedCylindersDataSize[materialId];
                }
                break;
                case GT_CONE:
                {
                    primitive->serializeData(_serializedConesData[materialId]);
                    ++_serializedConesDataSize[materialId];
                }
                default:
                    break;
            }
        }

        _buildParametricOSPGeometry( materialId );

        // Triangle mesh
        OSPGeometry mesh = ospNewGeometry("trianglemesh");
        assert(mesh);
        OSPData vertices = ospNewData(
            _trianglesMeshes[materialId].getVertices().size(),
            OSP_FLOAT3A,
            &_trianglesMeshes[materialId].getVertices()[0],
            OSP_DATA_SHARED_BUFFER);
        OSPData normals = ospNewData(
            _trianglesMeshes[materialId].getNormals().size(),
            OSP_FLOAT3A,
            &_trianglesMeshes[materialId].getNormals()[0],
            OSP_DATA_SHARED_BUFFER);
        OSPData indices = ospNewData(
            _trianglesMeshes[materialId].getIndices().size(),
            OSP_INT3,
            &_trianglesMeshes[materialId].getIndices()[0],
            OSP_DATA_SHARED_BUFFER);
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
        ospAddGeometry(_scene, mesh);
    }

    _commitLights();

    if(!_geometryParameters.getLoadCacheFile().empty())
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
    BRAYNS_INFO << "Geometry information" << std::endl;
    BRAYNS_INFO << "Spheres  : " << totalNbSpheres << std::endl;
    BRAYNS_INFO << "Cylinders: " << totalNbCylinders << std::endl;
    BRAYNS_INFO << "Cones    : " << totalNbCones << std::endl;
    BRAYNS_INFO << "--------------------" << std::endl;

    if(!_geometryParameters.getSaveCacheFile().empty())
        _saveCacheFile();
}

void OSPRayScene::_commitLights()
{
    for( auto renderer: _renderers )
    {
        OSPRayRenderer* osprayRenderer =
            dynamic_cast< OSPRayRenderer* >( renderer.second.get( ));

        std::vector< OSPLight > ospLights;
        for( auto light: _lights )
        {
            OSPLight ospLight;
            DirectionalLight* directionalLight =
                dynamic_cast< DirectionalLight* >( light.get( ));
            if( directionalLight != 0 )
            {
                ospLight =
                    ospNewLight( osprayRenderer->impl(), "DirectionalLight" );
                const Vector3f color = directionalLight->getColor( );
                ospSet3f( ospLight, "color",
                    color.x( ), color.y( ), color.z( ));
                const Vector3f direction = directionalLight->getDirection( );
                ospSet3f( ospLight, "direction",
                    direction.x( ), direction.y( ), direction.z( ));
                ospSet1f( ospLight, "intensity",
                    directionalLight->getIntensity( ));
                ospCommit( ospLight );
                ospLights.push_back( ospLight );
            }
            else
            {
                PointLight* pointLight = dynamic_cast< PointLight* >( light.get( ));
                if( pointLight != 0 )
                {
                    ospLight =
                        ospNewLight( osprayRenderer->impl(), "PointLight" );
                    const Vector3f position = pointLight->getPosition( );
                    ospSet3f( ospLight, "position",
                        position.x( ), position.y( ), position.z( ));
                    const Vector3f color = pointLight->getColor( );
                    ospSet3f( ospLight, "color",
                        color.x( ), color.y( ), color.z( ));
                    ospSet1f( ospLight, "intensity",
                        pointLight->getIntensity( ));
                    ospSet1f( ospLight, "radius",
                        pointLight->getCutoffDistance( ));
                    ospCommit( ospLight );
                    ospLights.push_back( ospLight );
                }
            }
        }
        _ospLightData = ospNewData( _lights.size(), OSP_OBJECT, &ospLights[0] );
        ospCommit( _ospLightData );
        ospSetData( osprayRenderer->impl(), "lights", _ospLightData );
    }
}

void OSPRayScene::_commitMaterials()
{
    for( auto renderer: _renderers )
    {
        OSPRayRenderer* osprayRenderer =
            dynamic_cast<OSPRayRenderer*>( renderer.second.get( ));
        for( size_t index=0; index < _materials.size(); ++index )
        {
            if( !_ospMaterials[index] )
            {
                _ospMaterials[index] =
                    ospNewMaterial(osprayRenderer->impl(),
                        "ExtendedOBJMaterial");
            }

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
            ospCommit(ospMaterial);

            if( index == MATERIAL_SIMULATION )
            {
                Texture2DPtr simulationTexture =
                    _textures[TEXTURE_NAME_SIMULATION];
                if( simulationTexture )
                {
                    _ospMaterialData =
                        ospNewData( 1, OSP_OBJECT, &_ospMaterials[index] );
                    ospCommit( _ospMaterialData );
                    ospSetData( osprayRenderer->impl(),
                        "material", _ospMaterialData );

                    ospSet1i( osprayRenderer->impl(), "simulationNbFrames",
                        simulationTexture->getHeight());
                    ospSet1i( osprayRenderer->impl(), "simulationNbOffsets",
                        simulationTexture->getWidth());
                    ospCommit(osprayRenderer->impl());
                    BRAYNS_DEBUG << "Simulation material set: " <<
                        simulationTexture->getWidth() << "x" <<
                        simulationTexture->getHeight() << std::endl;
                }
            }
        }
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
      if(texture->getNbChannels() == 3) type = OSP_TEXTURE_SRGB; // maybe OSP_TEXTURE_RGBA8
      if(texture->getNbChannels() == 4) type = OSP_TEXTURE_SRGBA; // maybe OSP_TEXTURE_RGB8
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
