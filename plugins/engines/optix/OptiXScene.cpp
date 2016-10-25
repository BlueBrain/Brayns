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

#include "OptiXUtils.h"
#include "OptiXScene.h"

#include <brayns/common/log.h>
#include <brayns/common/light/DirectionalLight.h>
#include <brayns/common/light/PointLight.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/common/volume/VolumeHandler.h>

namespace
{
    const std::string DEFAULT_ACCELERATION_STRUCTURE = "Trbvh";
    const std::string CUDA_SPHERES = "Spheres.cu";
    const std::string CUDA_CYLINDERS = "Cylinders.cu";
    const std::string CUDA_CONES = "Cones.cu";
    const std::string CUDA_TRIANGLES_MESH = "TrianglesMesh.cu";
    const std::string CUDA_FUNCTION_BOUNDS = "bounds";
    const std::string CUDA_FUNCTION_INTERSECTION = "intersect";
    const std::string CUDA_FUNCTION_ROBUST_INTERSECTION = "robust_intersect";
}

namespace brayns
{

OptiXScene::OptiXScene(
    Renderers renderer,
    ParametersManager& parametersManager,
    optix::Context& context )
    : Scene( renderer, parametersManager )
    , _context( context )
    , _lightBuffer( 0 )
    , _accelerationStructure( DEFAULT_ACCELERATION_STRUCTURE )
    , _colorMapBuffer( 0 )
    , _mesh( 0 )
    , _verticesBuffer( 0 )
    , _indicesBuffer( 0 )
    , _normalsBuffer( 0 )
    , _textureCoordsBuffer( 0 )
    , _materialsBuffer( 0 )
    , _volumeBuffer( 0 )
{
}

OptiXScene::~OptiXScene()
{
    if( _volumeBuffer ) _volumeBuffer->destroy();
    if( _colorMapBuffer ) _colorMapBuffer->destroy();
    for( auto buffer: _spheresBuffers )
        buffer.second->destroy();
    for( auto buffer: _cylindersBuffers )
        buffer.second->destroy();
    for( auto buffer: _conesBuffers )
        buffer.second->destroy();
    if( _verticesBuffer ) _verticesBuffer->destroy();
    if( _indicesBuffer ) _indicesBuffer->destroy();
    if( _normalsBuffer ) _normalsBuffer->destroy();
    if( _textureCoordsBuffer ) _textureCoordsBuffer->destroy();
    if( _materialsBuffer ) _materialsBuffer->destroy();
}

void OptiXScene::commit()
{
}

void OptiXScene::buildGeometry()
{

    BRAYNS_INFO << "Building OptiX geometry" << std::endl;

    // Make sure lights and materials have been initialized before assigning
    // the geometry
    commitMaterials();
    commitLights();

    _geometryInstances.clear();

    BRAYNS_INFO << "----------------------------------------" << std::endl;
    BRAYNS_INFO << "Data information:" << std::endl;

    uint64_t paramGeomMemSize = _processParametricGeometries();
    uint64_t meshMemSize = _processMeshes();
    uint64_t totalGPUMemory = paramGeomMemSize + meshMemSize;

    BRAYNS_INFO << "Total GPU : " << totalGPUMemory << " bytes ("
                << totalGPUMemory / 1048576 << " MB)"
                << std::endl;
    BRAYNS_INFO << "----------------------------------------" << std::endl;

    // Geometry hierarchy
    _geometryGroup = _context->createGeometryGroup();
    _geometryGroup->setAcceleration(
        _context->createAcceleration( _accelerationStructure, _accelerationStructure ));
    _geometryGroup->setChildCount( _geometryInstances.size() );

    BRAYNS_INFO << "Adding " << _geometryInstances.size() << " geometry instances" << std::endl;
    for( size_t i = 0; i < _geometryInstances.size(); ++ i)
        _geometryGroup->setChild( i, _geometryInstances[i] );

    BRAYNS_INFO << "Acceleration size: "
                << _geometryGroup->getAcceleration()->getDataSize()
                << std::endl;

    _context["top_object"]->set( _geometryGroup );
    _context["top_shadower"]->set( _geometryGroup );

    _context->validate();
}

void OptiXScene::commitLights()
{
    _optixLights.clear();
    if( _lights.size() == 0 )
    {
        BRAYNS_ERROR << "No lights are currently defined" << std::endl;
        return;
    }

    for( auto light: _lights )
    {
        PointLight* pointLight = dynamic_cast< PointLight* >( light.get( ));
        if( pointLight != 0 )
        {
            const Vector3f& position = pointLight->getPosition( );
            const Vector3f& color = pointLight->getColor( );
            BasicLight optixLight =
            {
                { position.x(),position.y(), position.z( ) },
                { color.x(), color.y(), color.z() },
                1, // Casts shadows
                LT_POINT
            };
            _optixLights.push_back( optixLight );
        }
        else
        {
            DirectionalLight* directionalLight = dynamic_cast< DirectionalLight* >( light.get( ));
            if( directionalLight )
            {
                const Vector3f& direction = directionalLight->getDirection( );
                const Vector3f& color = directionalLight->getColor( );
                BasicLight optixLight =
                {
                    { direction.x(),direction.y(), direction.z( ) },
                    { color.x(), color.y(), color.z() },
                    1, // Casts shadows
                    LT_DIRECTIONAL
                };
                _optixLights.push_back( optixLight );
            }
        }
    }

    if( !_lightBuffer )
    {
        _lightBuffer = _context->createBuffer( RT_BUFFER_INPUT );
        _lightBuffer->setFormat( RT_FORMAT_USER );
        _lightBuffer->setElementSize( sizeof( BasicLight ) );
        _lightBuffer->setSize( _optixLights.size() );
        _context[ "lights" ]->set( _lightBuffer );
    }

    memcpy( _lightBuffer->map(), &_optixLights[0], _optixLights.size() * sizeof( BasicLight ));
    _lightBuffer->unmap();
}

uint64_t OptiXScene::_processParametricGeometries()
{
    // Compile Kernels
    const std::string spherePtx = getPTXPath( CUDA_SPHERES );
    optix::Program spheresBoundsProgram =
        _context->createProgramFromPTXFile( spherePtx, CUDA_FUNCTION_BOUNDS );
    optix::Program spheresIntersectProgram =
        _context->createProgramFromPTXFile( spherePtx, CUDA_FUNCTION_INTERSECTION );

    const std::string cylinderPtx = getPTXPath( CUDA_CYLINDERS );
    optix::Program cylindersBoundsProgram =
        _context->createProgramFromPTXFile( cylinderPtx, CUDA_FUNCTION_BOUNDS );
    optix::Program cylindersIntersectProgram =
        _context->createProgramFromPTXFile( cylinderPtx, CUDA_FUNCTION_INTERSECTION );

    const std::string conesPtx = getPTXPath( CUDA_CONES );
    optix::Program conesBoundsProgram =
        _context->createProgramFromPTXFile( conesPtx, CUDA_FUNCTION_BOUNDS );
    optix::Program conesIntersectProgram =
        _context->createProgramFromPTXFile( conesPtx, CUDA_FUNCTION_INTERSECTION );

    // Load geometry to GPU
    size_t totalNbSpheres = 0;
    size_t totalNbCylinders = 0;
    size_t totalNbCones = 0;
    for( size_t materialId = 0; materialId < _materials.size(); ++materialId )
    {
        _timestampSpheresIndices[ materialId ] = 0;
        _timestampCylindersIndices[ materialId ] = 0;
        _timestampConesIndices[ materialId ] = 0;
        if( _primitives.find( materialId ) != _primitives.end( ))
        {
            for( const auto& primitive: _primitives[materialId] )
            {
                switch( primitive->getGeometryType() )
                {
                    case GT_SPHERE:
                    {
                        primitive->serializeData( _serializedSpheresData[ materialId ] );
                        ++_serializedSpheresDataSize[ materialId ];
                        ++_timestampSpheresIndices[ materialId ];
                    }
                    break;
                    case GT_CYLINDER:
                    {
                        primitive->serializeData( _serializedCylindersData[ materialId ] );
                        ++_serializedCylindersDataSize[ materialId ];
                        ++_timestampCylindersIndices[ materialId ];
                    }
                    break;
                    case GT_CONE:
                    {
                        primitive->serializeData( _serializedConesData[ materialId ] );
                        ++_serializedConesDataSize[ materialId ];
                        ++_timestampConesIndices[ materialId ];
                    }
                    default:
                        break;
                }
            }

            totalNbSpheres += _timestampSpheresIndices[ materialId ];
            totalNbCylinders += _timestampCylindersIndices[ materialId ];
            totalNbCones += _timestampConesIndices[ materialId ];
            _primitives[materialId].clear();
        }

        // Create spheres geometry
        if( _timestampSpheresIndices[ materialId ] != 0 )
        {
            _optixSpheres[ materialId ] = _context->createGeometry();
            _optixSpheres[ materialId ]->setPrimitiveCount( _timestampSpheresIndices[ materialId ] );
            _optixSpheres[ materialId ]->setBoundingBoxProgram( spheresBoundsProgram );
            _optixSpheres[ materialId ]->setIntersectionProgram( spheresIntersectProgram );
            uint64_t size = _timestampSpheresIndices[ materialId ] *
                Sphere::getSerializationSize();
            _spheresBuffers[ materialId ] =
                _context->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT, size );
            memcpy(
                _spheresBuffers[ materialId ]->map(),
                &_serializedSpheresData[ materialId ][ 0 ],
                size * sizeof(float));
            _spheresBuffers[ materialId ]->unmap();
            _optixSpheres[ materialId ][ "spheres" ]->setBuffer(
                _spheresBuffers[ materialId ] );
            _geometryInstances.push_back(
                _context->createGeometryInstance(
                    _optixSpheres[ materialId ],
                    &_optixMaterials[ materialId ],
                    &_optixMaterials[ materialId ]+1 ) );
            _serializedSpheresData[ materialId ].clear();
        }

        // Create cylinders geometry
        if( _timestampCylindersIndices[ materialId ] != 0 )
        {
            _optixCylinders[ materialId ] = _context->createGeometry();
            _optixCylinders[ materialId ]->setPrimitiveCount( _timestampCylindersIndices[ materialId ] );
            _optixCylinders[ materialId ]->setBoundingBoxProgram( cylindersBoundsProgram );
            _optixCylinders[ materialId ]->setIntersectionProgram( cylindersIntersectProgram );
            uint64_t size = _timestampCylindersIndices[ materialId ] *
                Cylinder::getSerializationSize();
            _cylindersBuffers[ materialId ] =
                _context->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT, size );
            memcpy(
                _cylindersBuffers[ materialId ]->map(),
                &_serializedCylindersData[ materialId ][ 0 ],
                size * sizeof(float));
            _cylindersBuffers[ materialId ]->unmap();
            _optixCylinders[ materialId ][ "cylinders" ]->setBuffer(
                _cylindersBuffers[ materialId ] );
            _geometryInstances.push_back(
                _context->createGeometryInstance(
                    _optixCylinders[ materialId ],
                    &_optixMaterials[ materialId ],
                    &_optixMaterials[ materialId ]+1 ) );
            _serializedCylindersData[ materialId ].clear();
        }

        // Create cones geometry
        if( _timestampConesIndices[ materialId ] != 0 )
        {
            _optixCones[ materialId ] = _context->createGeometry();
            _optixCones[ materialId ]->setPrimitiveCount( _timestampConesIndices[ materialId ] );
            _optixCones[ materialId ]->setBoundingBoxProgram( conesBoundsProgram );
            _optixCones[ materialId ]->setIntersectionProgram( conesIntersectProgram );
            uint64_t size = _timestampConesIndices[ materialId ] *
                Cone::getSerializationSize();
            _conesBuffers[ materialId ] =
                _context->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT, size );
            memcpy(
                _conesBuffers[ materialId ]->map(),
                &_serializedConesData[ materialId ][ 0 ],
                size * sizeof(float) );
            _conesBuffers[ materialId ]->unmap();
            _optixCones[ materialId ][ "cones" ]->setBuffer(
                _conesBuffers[ materialId ] );
            _geometryInstances.push_back(
                _context->createGeometryInstance(
                    _optixCones[ materialId ],
                    &_optixMaterials[ materialId ],
                    &_optixMaterials[ materialId ]+1 ) );
            _serializedConesData[ materialId ].clear();
         }
    }

    const uint64_t spheresMemSize =
        totalNbSpheres * Sphere::getSerializationSize() * sizeof(float);
    const uint64_t cylindersMemSize =
        totalNbCylinders * Cylinder::getSerializationSize() * sizeof(float);
    const uint64_t conesMemSize =
        totalNbCones * Cone::getSerializationSize() * sizeof(float);
    const uint64_t bvhSize = _getBvhSize( totalNbSpheres + totalNbCylinders + totalNbCones );

    BRAYNS_INFO << "- Spheres   : " << totalNbSpheres
                << " [" << spheresMemSize << " bytes]"
                << std::endl;
    BRAYNS_INFO << "- Cylinders : " << totalNbCylinders
                << " [" << cylindersMemSize << " bytes]"
                << std::endl;
    BRAYNS_INFO << "- Cones     : " << totalNbCones
                << " [" << conesMemSize << " bytes]"
                << std::endl;
    return bvhSize + spheresMemSize + cylindersMemSize + conesMemSize;
}

uint64_t OptiXScene::_processMeshes()
{
    if( getVolumeHandler() )
    {
        // Optix needs a bounding box around the volume so that if can find intersections
        // before initiating the traversal
        _processVolumeAABBGeometry();
    }

    // These buffers will be shared across all Geometries
    uint64_t size = 0;

    uint64_t nbTotalVertices = 0;
    uint64_t nbTotalIndices = 0;
    uint64_t nbTotalNormals = 0;
    uint64_t nbTotalTexCoords = 0;
    uint64_t nbTotalMaterials = 0;

    for( size_t materialId = 0; materialId < _materials.size(); ++materialId )
    {
        nbTotalVertices += _trianglesMeshes[ materialId ].getVertices().size();
        nbTotalIndices += _trianglesMeshes[ materialId ].getIndices().size();
        nbTotalNormals += _trianglesMeshes[ materialId ].getNormals().size();
        nbTotalTexCoords += _trianglesMeshes[ materialId ].getTextureCoordinates().size();
    }

    Vector3fs vertices;
    Vector3uis indices;
    Vector3fs normals;
    Vector2fs texCoords;
    ints materials;

    if( nbTotalIndices == 0 )
    {
        BRAYNS_INFO << "- No meshes" << std::endl;
        return 0;
    }

    uint64_t offset = 0;
    for( size_t materialId = 0; materialId < _materials.size(); ++materialId )
    {
        if( _trianglesMeshes[ materialId ].getIndices().size() != 0 )
        {
            // Vertices
            const Vector3fs& v = _trianglesMeshes[ materialId ].getVertices();
            vertices.insert( vertices.end(), v.begin(), v.end() );

            // Indices
            for( size_t i = 0; i < _trianglesMeshes[ materialId ].getIndices().size(); ++i)
            {
                Vector3ui value = _trianglesMeshes[ materialId ].getIndices()[i];
                value += offset;
                indices.push_back( value );
                materials.push_back( materialId );
            }
            _trianglesMeshes[ materialId ].getIndices().clear();

            // Normals
            const Vector3fs& n = _trianglesMeshes[ materialId ].getNormals();
            normals.insert( normals.end(), n.begin(), n.end() );
            _trianglesMeshes[ materialId ].getNormals().clear();

            // Texture coordinates
            const Vector2fs& t = _trianglesMeshes[ materialId ].getTextureCoordinates();
            texCoords.insert( texCoords.end(), t.begin(), t.end() );
            _trianglesMeshes[ materialId ].getTextureCoordinates().clear();

            offset += _trianglesMeshes[ materialId ].getVertices().size();
            _trianglesMeshes[ materialId ].getVertices().clear();
        }
    }

    _verticesBuffer = _context->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, vertices.size() );
    size = vertices.size() * 3 * sizeof(float);
    if( size != 0 )
    {
        memcpy( _verticesBuffer->map(), &vertices.data()[0], size );
        _verticesBuffer->unmap();
    }

    _indicesBuffer = _context->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_INT3, indices.size() );
    size = indices.size() * 3 * sizeof(int);
    if( size != 0 )
    {
        memcpy( _indicesBuffer->map(), &indices.data()[0], size );
        _indicesBuffer->unmap();
    }

    _normalsBuffer = _context->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, normals.size() );
    size = normals.size() * 3 * sizeof(float);
    if( size != 0 )
    {
        memcpy( _normalsBuffer->map(), &normals.data()[0], size );
        _normalsBuffer->unmap();
    }

    _textureCoordsBuffer =
        _context->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_FLOAT2, texCoords.size() );
    size = texCoords.size() * 2 * sizeof(float);
    if( size != 0 )
    {
        memcpy( _textureCoordsBuffer->map(), &texCoords.data()[0], size );
        _textureCoordsBuffer->unmap();
    }

    size = materials.size() * sizeof(int);
    _materialsBuffer = _context->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_INT, materials.size() );
    if( size != 0 )
    {
        memcpy( _materialsBuffer->map(), &materials.data()[0], size );
        _materialsBuffer->unmap();
    }
    nbTotalMaterials = materials.size();

    // Attach all of the buffers at the Context level since they are shared
    _context[ "vertices_buffer" ]->setBuffer( _verticesBuffer );
    _context[ "indices_buffer" ]->setBuffer( _indicesBuffer );
    _context[ "normal_buffer" ]->setBuffer( _normalsBuffer );
    _context[ "texcoord_buffer" ]->setBuffer( _textureCoordsBuffer );
    _context[ "material_buffer" ]->setBuffer( _materialsBuffer );

    std::string trianglesPtx = getPTXPath( "TrianglesMesh.cu" );
    optix::Program meshBoundsProgram =
        _context->createProgramFromPTXFile( trianglesPtx, CUDA_FUNCTION_BOUNDS );
    optix::Program meshIntersectProgram =
        _context->createProgramFromPTXFile( trianglesPtx, CUDA_FUNCTION_INTERSECTION );

    _mesh = _context->createGeometry();
    _mesh->setIntersectionProgram( meshIntersectProgram );
    _mesh->setBoundingBoxProgram( meshBoundsProgram );
    _mesh->setPrimitiveCount( nbTotalIndices );

    _geometryInstances.push_back(
        _context->createGeometryInstance(
            _mesh,
            &_optixMaterials[ 0 ],
            &_optixMaterials[ 0 ] + _optixMaterials.size() ) );

    const uint64_t verticesMemSize = nbTotalVertices * 3 * sizeof(float);
    const uint64_t indicesMemSize = nbTotalIndices * 3 * sizeof(int);
    const uint64_t normalsMemSize = nbTotalNormals * 3 * sizeof(float);
    const uint64_t texCoordsMemSize = nbTotalTexCoords * 2 * sizeof(float);
    const uint64_t materialsMemSize = nbTotalMaterials * sizeof(int);

    BRAYNS_INFO << "- Vertices  : " << vertices.size()
                << " [" << verticesMemSize << " bytes]"
                << std::endl;
    BRAYNS_INFO << "- Indices   : " << indices.size()
                << " [" << indicesMemSize << " bytes]"
                << std::endl;
    BRAYNS_INFO << "- Normals   : " << normals.size()
                << " [" << normalsMemSize << " bytes]"
                << std::endl;
    BRAYNS_INFO << "- Tex coords: " << texCoords.size()
                << " [" << texCoordsMemSize << " bytes]"
                << std::endl;
    BRAYNS_INFO << "- Materials : " << materials.size()
                << " [" << materialsMemSize << " bytes]"
                << std::endl;

    const uint64_t bvhSize = _getBvhSize( indicesMemSize / 3 );

    return bvhSize + verticesMemSize + indicesMemSize +
           normalsMemSize + texCoordsMemSize + materialsMemSize;
}

void OptiXScene::commitMaterials( const bool updateOnly )
{
    BRAYNS_INFO << "Commit OptiX materials" << std::endl;

    const std::string phongPtx = getPTXPath( "Phong.cu" );
    optix::Program phong_ch =
        _context->createProgramFromPTXFile( phongPtx, "closest_hit_radiance" );
    optix::Program phong_ah =
        _context->createProgramFromPTXFile( phongPtx, "any_hit_shadow" );

    _optixMaterials.clear();
    _optixMaterials.resize( _materials.size( ));
    for( size_t materialId = 0; materialId < _materials.size(); ++materialId )
    {
        // Material
        MaterialPtr material = _materials[ materialId ];
        assert(material);

        optix::Material optixMaterial = 0;
        if( updateOnly )
            optixMaterial = _optixMaterials[ materialId ];
        else
            optixMaterial = _context->createMaterial();
        optixMaterial->setClosestHitProgram( 0, phong_ch );
        optixMaterial->setAnyHitProgram( 1, phong_ah );

        const Vector3f& color = material->getColor();
        optixMaterial["Kd"]->setFloat( color.z(), color.y(), color.x() );
        const Vector3f& specularColor = material->getSpecularColor();
        optixMaterial["Ks"]->setFloat( specularColor.z(), specularColor.y(), specularColor.x( ));
        optixMaterial["phong_exp"]->setFloat( material->getSpecularExponent( ));
        const float reflectionIndex = material->getReflectionIndex();
        optixMaterial["Kr"]->setFloat( reflectionIndex,  reflectionIndex,  reflectionIndex);
        const float opacity = material->getOpacity();
        optixMaterial["Ko"]->setFloat( opacity, opacity, opacity );
        optixMaterial["refraction_index"]->setFloat( material->getRefractionIndex( ));

        if( !updateOnly )
            _optixMaterials[ materialId ] = optixMaterial;
    }
}

void OptiXScene::commitSimulationData()
{
    BRAYNS_ERROR << "OptiXScene::commitSimulationData not implemented" << std::endl;
}

void OptiXScene::commitVolumeData()
{
    const float timestamp = _parametersManager.getSceneParameters().getTimestamp();
    VolumeHandlerPtr volumeHandler = getVolumeHandler();
    if( !volumeHandler )
    {
        _volumeBuffer = _context->createBuffer( RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE, 0 );
        _context[ "volumeData" ]->setBuffer( _volumeBuffer );
        _context[ "volumeDimensions" ]->setUint( 0, 0, 0 );
        _context[ "volumeScale" ]->setFloat( 0.f, 0.f, 0.f );
        _context[ "volumePosition" ]->setFloat( 0.f, 0.f, 0.f );
        _context[ "volumeEpsilon" ]->setFloat( 0.f );
        _context[ "volumeDiag" ]->setFloat( 0.f );
        return;
    }

    volumeHandler->setTimestamp( timestamp );
    void* data = volumeHandler->getData();
    if( data )
    {
        if( _volumeBuffer ) _volumeBuffer->destroy();

        _volumeBuffer = _context->createBuffer(
            RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_BYTE,
            volumeHandler->getSize());
        uint64_t size = volumeHandler->getSize() * sizeof( unsigned char );
        if( size != 0 )
        {
            memcpy( _volumeBuffer->map(), data, size );
            _volumeBuffer->unmap();
        }
        _context[ "volumeData" ]->setBuffer( _volumeBuffer );
    }

    const Vector3ui& dimensions = volumeHandler->getDimensions();
    _context[ "volumeDimensions" ]->setUint( dimensions.x(), dimensions.y(), dimensions.z());

    const Vector3f& elementSpacing = volumeHandler->getElementSpacing();
    _context[ "volumeElementSpacing" ]->setFloat(
        elementSpacing.x(), elementSpacing.y(), elementSpacing.z());

    const Vector3f& position = volumeHandler->getOffset();
    _context[ "volumeOffset" ]->setFloat( position.x(), position.y(), position.z());

    _context[ "volumeEpsilon" ]->setFloat(
        volumeHandler->getEpsilon(
            elementSpacing, _parametersManager.getVolumeParameters().getSamplesPerRay()));

    const Vector3f diag = Vector3f( dimensions ) * elementSpacing;
    const float volumeDiag = diag.find_max();
    _context[ "volumeDiag" ]->setFloat( volumeDiag );
}

void OptiXScene::commitTransferFunctionData()
{
    if( _colorMapBuffer == 0 )
        _colorMapBuffer = _context->createBuffer(
            RT_BUFFER_INPUT, RT_FORMAT_FLOAT4, _transferFunction.getDiffuseColors().size() );

    uint64_t size = _transferFunction.getDiffuseColors().size() * 4 * sizeof( float );
    if( size != 0 )
    {
        memcpy( _colorMapBuffer->map(), &_transferFunction.getDiffuseColors()[0], size );
        _colorMapBuffer->unmap();
    }

    _context[ "colorMap" ]->setBuffer( _colorMapBuffer );
    _context[ "colorMapMinValue" ]->setFloat( _transferFunction.getValuesRange().x() );
    _context[ "colorMapRange" ]->setFloat(
        _transferFunction.getValuesRange().y() - _transferFunction.getValuesRange().x() );
    _context[ "colorMapSize" ]->setUint( _transferFunction.getDiffuseColors().size() );

}

void OptiXScene::_processVolumeAABBGeometry()
{
    VolumeHandlerPtr volumeHandler = getVolumeHandler();
    if( !volumeHandler )
        return;

    const Vector3f positions[ 8 ] =
    {
        {  0.f, 0.f, 0.f },
        {  1.f, 0.f, 0.f }, //    6--------7
        {  0.f, 1.f, 0.f }, //   /|       /|
        {  1.f, 1.f, 0.f }, //  2--------3 |
        {  0.f, 0.f, 1.f }, //  | |      | |
        {  1.f, 0.f, 1.f }, //  | 4------|-5
        {  0.f, 1.f, 1.f }, //  |/       |/
        {  1.f, 1.f, 1.f }  //  0--------1
    };

    const uint16_t indices[ 6 ][ 6 ] =
    {
        { 0, 1, 3, 3, 2, 0 }, // Front
        { 1, 5, 7, 7, 3, 1 }, // Right
        { 5, 4, 6, 6, 7, 5 }, // Back
        { 4, 0, 2, 2, 6, 4 }, // Left
        { 0, 1, 5, 5, 4, 0 }, // Bottom
        { 2, 3, 7, 7, 6, 2 }  // Top
    };

    const float timestamp = _parametersManager.getSceneParameters().getTimestamp();
    volumeHandler->setTimestamp( timestamp );
    const Vector3f& volumeElementSpacing = volumeHandler->getElementSpacing();
    const Vector3f& volumeOffset = volumeHandler->getOffset();
    const Vector3ui& volumeDimensions = volumeHandler->getDimensions();

    uint64_t offset = _trianglesMeshes[ MATERIAL_INVISIBLE ].getVertices().size();
    for( size_t face = 0; face < 6; ++face )
    {
        for( size_t index = 0; index < 6; ++index )
        {
            const Vector3f position =
                positions[ indices[face][index] ]  * volumeElementSpacing * volumeDimensions +
                volumeOffset;

            _trianglesMeshes[ MATERIAL_INVISIBLE ].getVertices().push_back( position );

            _bounds.merge( position );
        }
        const size_t index = offset + face * 6;
        _trianglesMeshes[ MATERIAL_INVISIBLE ].getIndices().push_back(
            Vector3ui( index + 0, index + 1, index + 2 ));
        _trianglesMeshes[ MATERIAL_INVISIBLE ].getIndices().push_back(
            Vector3ui( index + 3, index + 4, index + 5 ));
    }
}

uint64_t OptiXScene::_getBvhSize( const uint64_t nbElements ) const
{
    // An estimate of the BVH size
    return 64 * 1.3f * nbElements;
}

}
