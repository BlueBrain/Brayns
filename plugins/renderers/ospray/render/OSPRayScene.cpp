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

#include "OSPRayScene.h"
#include "OSPRayRenderer.h"

#include <brayns/common/log.h>
#include <brayns/common/parameters/GeometryParameters.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/common/light/DirectionalLight.h>
#ifdef BRAYNS_USE_MAGICKCPP
#  include <plugins/loaders/TextureLoader.h>
#endif

namespace brayns
{

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

OSPRayScene::OSPRayScene(RendererPtr renderer, GeometryParameters& geometryParameters)
    : Scene(renderer, geometryParameters),
    _scene(ospNewModel())
{
}

void OSPRayScene::commit()
{
    ospCommit(_scene);
}

void OSPRayScene::loadData()
{
    if(!_geometryParameters.getSWCFolder().empty())
        loadSWCFolder();

    if(!_geometryParameters.getPDBFolder().empty())
        loadPDBFolder();

    if(!_geometryParameters.getH5Folder().empty())
        loadH5Folder();

    if(!_geometryParameters.getMeshFolder().empty())
        loadMeshFolder();
}

void OSPRayScene::buildGeometry()
{
    // Make sure lights and materials have been initialized before assigning
    // the geometry
    _commitMaterials();

    BRAYNS_INFO << "Building OSPRay geometry" << std::endl;

    OSPGeometry* extendedSpheres = new OSPGeometry[_materials.size()];
    OSPGeometry* extendedCylinders = new OSPGeometry[_materials.size()];

    for( size_t materialId=0; materialId<_materials.size(); ++materialId )
    {
        size_t nbSpheres = 0;
        std::vector<float> serializedSpheresData;
        size_t nbCylinders = 0;
        std::vector<float> serializedCylindersData;
        for( const PrimitivePtr& primitive: _primitives[materialId] )
        {
            switch(primitive->getGeometryType())
            {
                case GT_SPHERE:
                {
                    primitive->serializeData(serializedSpheresData);
                    ++nbSpheres;
                }
                break;
                case GT_CYLINDER:
                {
                    primitive->serializeData(serializedCylindersData);
                    ++nbCylinders;
                }
                break;
                case GT_CONE:
                default:
                    break;
            }
        }

        // Extended spheres
        extendedSpheres[materialId] = ospNewGeometry("extendedspheres");
        OSPData data = ospNewData(nbSpheres*5, OSP_FLOAT, &serializedSpheresData[0]);
        ospSetObject(extendedSpheres[materialId], "extendedspheres", data);
        ospSet1i(extendedSpheres[materialId], "bytes_per_extended_sphere", 5*sizeof(float));
        ospSet1i(extendedSpheres[materialId], "offset_radius", 3*sizeof(float));
        ospSet1i(extendedSpheres[materialId], "offset_frame", 4*sizeof(float));

        if(_ospMaterials[materialId])
            ospSetMaterial(extendedSpheres[materialId], _ospMaterials[materialId]);

        ospCommit(extendedSpheres[materialId]);
        ospAddGeometry(_scene, extendedSpheres[materialId]);

        // Extended cylinders
        extendedCylinders[materialId] = ospNewGeometry("extendedcylinders");
        assert(extendedCylinders[materialId]);

        data = ospNewData(nbCylinders*8, OSP_FLOAT, &serializedCylindersData[0]);
        ospSetObject(extendedCylinders[materialId], "extendedcylinders", data);
        ospSet1i(extendedCylinders[materialId], "bytes_per_extended_cylinder", 8*sizeof(float));
        ospSet1i(extendedCylinders[materialId], "offset_frame", 7*sizeof(float));

        if(_ospMaterials[materialId])
            ospSetMaterial(extendedCylinders[materialId], _ospMaterials[materialId]);

        ospCommit(extendedCylinders[materialId]);
        ospAddGeometry(_scene, extendedCylinders[materialId]);

        // Triangle mesh
        OSPGeometry mesh = ospNewGeometry("trianglemesh");
        assert(mesh);
        OSPData vertices = ospNewData(
            _trianglesMeshes[materialId].getVertices().size(),OSP_FLOAT3A,
            &_trianglesMeshes[materialId].getVertices()[0],OSP_DATA_SHARED_BUFFER);
        OSPData normals = ospNewData(
            _trianglesMeshes[materialId].getNormals().size(),OSP_FLOAT3A,
            &_trianglesMeshes[materialId].getNormals()[0],OSP_DATA_SHARED_BUFFER);
        OSPData indices = ospNewData(
            _trianglesMeshes[materialId].getIndices().size(),OSP_INT3,
            &_trianglesMeshes[materialId].getIndices()[0],OSP_DATA_SHARED_BUFFER);
        OSPData colors = ospNewData(
            _trianglesMeshes[materialId].getColors().size(),OSP_FLOAT3A,
            &_trianglesMeshes[materialId].getColors()[0],OSP_DATA_SHARED_BUFFER);
        OSPData texcoord = ospNewData(
            _trianglesMeshes[materialId].getTextureCoordinates().size(),OSP_FLOAT2,
            &_trianglesMeshes[materialId].getTextureCoordinates()[0],OSP_DATA_SHARED_BUFFER);
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
    delete extendedSpheres;
    delete extendedCylinders;

    _commitLights();
}

void OSPRayScene::_commitLights()
{
    OSPRayRenderer* osprayRenderer =
        dynamic_cast< OSPRayRenderer* >( _renderer.get( ));

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

void OSPRayScene::_commitMaterials()
{
    OSPRayRenderer* osprayRenderer = dynamic_cast<OSPRayRenderer*>(_renderer.get());
    for(size_t index=0; index<_materials.size(); ++index)
    {
        if(!_ospMaterials[index])
        {
            _ospMaterials[index] =
                ospNewMaterial(osprayRenderer->impl(), "ExtendedOBJMaterial");
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

#ifdef BRAYNS_USE_MAGICKCPP
        // Textures
        for(auto texture: material->getTextures())
        {
            TextureLoader textureLoader;
            textureLoader.loadTexture(_textures, texture.first, texture.second);

            OSPTexture2D ospTexture = _createTexture2D(texture.second);
            ospSetObject(
                ospMaterial,
                textureTypeMaterialAttribute[texture.first].attribute.c_str(),
                ospTexture);

            BRAYNS_DEBUG << "OSPRay texture assigned to " <<
                textureTypeMaterialAttribute[texture.first].attribute <<
                " of material " << index << std::endl;
        }
#endif
        ospCommit(ospMaterial);
    }
}

OSPTexture2D OSPRayScene::_createTexture2D(const std::string& textureName)
{
    if(_ospTextures.find(textureName) != _ospTextures.end())
        return _ospTextures[textureName];

    Texture2DPtr texture = _textures[textureName];
    if(!texture)
    {
        BRAYNS_WARN << "Texture " << textureName << " is not in the cache" << std::endl;
        return nullptr;
    }

    OSPDataType type = OSP_VOID_PTR;
    if(texture->getDepth() == 1)
    {
      if(texture->getNbChannels() == 3) type = OSP_UCHAR3;
      if(texture->getNbChannels() == 4) type = OSP_UCHAR4;
    }
    else if(texture->getDepth() == 4)
    {
      if(texture->getNbChannels() == 3) type = OSP_FLOAT3;
      if(texture->getNbChannels() == 4) type = OSP_FLOAT3A;
    }

    BRAYNS_DEBUG << "Creating OSPRay texture from " << textureName << " :" <<
        texture->getWidth() << "x" << texture->getHeight() << "x" <<
        (int)type << std::endl;

    OSPTexture2D ospTexture = ospNewTexture2D(
        texture->getWidth(), texture->getHeight(),
        type, texture->getRawData(), 0);

    assert(ospTexture);
    ospCommit(ospTexture);

    _ospTextures[textureName] = ospTexture;

    return ospTexture;
}

}
