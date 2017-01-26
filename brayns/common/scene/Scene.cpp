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

#include "Scene.h"

#include <brayns/common/log.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/common/material/Material.h>
#include <brayns/io/TransferFunctionLoader.h>
#include <brayns/io/NESTLoader.h>
#include <brayns/common/volume/VolumeHandler.h>

#include <servus/uri.h>

#include <boost/filesystem.hpp>

namespace brayns
{

Scene::Scene(
    Renderers renderers,
    ParametersManager& parametersManager )
    : _parametersManager( parametersManager )
    , _renderers( renderers )
    , _volumeHandler( 0 )
    , _simulationHandler( 0 )
{
}

Scene::~Scene( )
{
}

void Scene::reset( )
{
    _primitives.clear( );
    _trianglesMeshes.clear( );
    _bounds.reset();
}

void Scene::setMaterials(
    const MaterialType materialType,
    const size_t nbMaterials )
{
    _materials.clear( );
    for( size_t i = 0; i < nbMaterials; ++i )
    {
        MaterialPtr material( new Material );
        material->setSpecularColor( Vector3f( 0.f, 0.f, 0.f ));

        // Special materials ( skybox, etc )
        switch( i )
        {
            case MATERIAL_BOUNDING_BOX:
                material->setColor( Vector3f( 1.f, 1.f, 1.f ));
                material->setEmission( 1.f );
                break;
            case MATERIAL_INVISIBLE:
                material->setOpacity( 0.f );
                material->setRefractionIndex( 1.f );
                material->setColor( Vector3f( 1.f, 1.f, 1.f ));
                material->setSpecularColor( Vector3f( 0.f, 0.f, 0.f ));
                break;
            default:
                break;
        }

        switch( materialType )
        {
        case MT_DEFAULT:
            switch( i )
            {
                case 0: // Default
                case 1: // Soma
                    material->setColor( Vector3f( 0.9f, 0.9f, 0.9f ));
                    break;
                case 2: // Axon
                    material->setColor( Vector3f( 0.2f, 0.2f, 0.8f ));
                    break;
                case 3: // Dendrite
                    material->setColor( Vector3f( 0.8f, 0.2f, 0.2f ));
                    break;
                case 4: // Apical dendrite
                    material->setColor( Vector3f( 0.8f, 0.2f, 0.8f ));
                    break;
                default:
                    material->setColor( Vector3f(
                        float( std::rand( ) % 255 ) / 255.f,
                        float( std::rand( ) % 255 ) / 255.f,
                        float( std::rand( ) % 255 ) / 255.f ));
            }
            break;
        case MT_GRADIENT:
            material->setColor( Vector3f(
                float( std::rand( ) % nbMaterials ) / float( nbMaterials ),
                1.f,
                0.f ));
            break;
        case MT_PASTEL_COLORS:
            material->setColor( Vector3f(
                0.5f + float( std::rand( ) % 127 ) / 255.f,
                0.5f + float( std::rand( ) % 127 ) / 255.f,
                0.5f + float( std::rand( ) % 127 ) / 255.f));
            break;
        case MT_RANDOM:
            material->setColor( Vector3f(
                float(rand( ) % 255 ) / 255.f,
                float(rand( ) % 255 ) / 255.f,
                float(rand( ) % 255 ) / 255.f));
            switch( rand( ) % 4 )
            {
            case 0:
                // Transparent
                material->setOpacity( float( std::rand( )%100 ) / 100.f );
                material->setRefractionIndex( 0.98f );
                material->setSpecularColor( Vector3f( 0.01f, 0.01f, 0.01f ));
                material->setSpecularExponent( 10.f );
            case 1:
                // Light emmitter
                material->setEmission(1.f);
            case 2:
                // Reflector
                material->setReflectionIndex(
                    float( std::rand( ) % 100 ) / 100.f );
                material->setSpecularColor(
                    Vector3f( 0.01f, 0.01f, 0.01f ));
                material->setSpecularExponent( 10.f );
            }
            break;
        case MT_SHADES_OF_GREY:
            float value = float( std::rand( ) % 255 ) / 255.f;
            material->setColor( Vector3f( value, value, value ));
            break;
        }
        _materials.push_back(material);
    }
    BRAYNS_INFO << nbMaterials << " materials set" << std::endl;
}

MaterialPtr Scene::getMaterial( size_t index )
{
    return _materials[index];
}

void Scene::buildDefault( )
{
    BRAYNS_INFO << "Building default Cornell Box scene" << std::endl;

    const Vector3f WHITE = { 1.f, 1.f, 1.f };

    const Vector3f positions[8] =
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

    const uint16_t indices[6][6] =
    {
        { 5, 4, 6, 6, 7, 5 }, // Front
        { 1, 5, 7, 7, 3, 1 }, // Right
        { 0, 1, 3, 3, 2, 0 }, // Back
        { 4, 0, 2, 2, 6, 4 }, // Left
        { 0, 1, 5, 5, 4, 0 }, // Bottom
        { 2, 3, 7, 7, 6, 2 }  // Top
    };

    const Vector3f colors[6] =
    {
        { 0.8f, 0.8f, 0.8f },
        { 1.f, 0.f, 0.f },
        { 0.8f, 0.8f, 0.8f },
        { 0.f, 1.f, 0.f },
        { 0.8f, 0.8f, 0.8f },
        { 0.8f, 0.8f, 0.8f }
    };

    // Cornell box
    for( size_t material = 1; material < 6; ++material )
    {
        _materials[material]->setColor( colors[ material ] );
        _materials[material]->setSpecularColor( WHITE );
        _materials[material]->setSpecularExponent( 10.f );
        _materials[material]->setReflectionIndex( material == 4 ? 0.8f : 0.f );
        _materials[material]->setOpacity( 1.f );
        for( size_t i = 0; i < 6; ++i )
        {
            const Vector3f& position = positions[ indices[ material ][ i ] ];
            _trianglesMeshes[material].getVertices().push_back( position );
            _bounds.merge( position );
        }
        _trianglesMeshes[material].getIndices().push_back(
            Vector3ui( 0, 1, 2 ));
        _trianglesMeshes[material].getIndices().push_back(
            Vector3ui( 3, 4, 5 ));
    }

    size_t material = 7;

    // Sphere
    _primitives[material].push_back( SpherePtr(
        new Sphere( material, Vector3f( 0.25f, 0.26f, 0.30f ), 0.25f, 0, 0 )));
    _materials[material]->setOpacity( 0.3f );
    _materials[material]->setRefractionIndex( 1.1f );
    _materials[material]->setSpecularColor( WHITE );
    _materials[material]->setSpecularExponent( 100.f );

    // Cylinder
    ++material;
    _primitives[material].push_back( CylinderPtr(
        new Cylinder( material,
            Vector3f( 0.25f, 0.126f, 0.75f ), Vector3f( 0.75f, 0.126f, 0.75f ),
            0.125f, 0, 0 )));
    _materials[material]->setColor( Vector3f( 0.1f, 0.1f, 0.8f ));
    _materials[material]->setSpecularColor( WHITE );
    _materials[material]->setSpecularExponent( 10.f );

    // Cone
    ++material;
    _primitives[material].push_back( ConePtr(
        new Cone( material,
            Vector3f( 0.75f, 0.01f, 0.25f ), Vector3f( 0.75f, 0.5f, 0.25f ),
            0.15f, 0.f, 0, 0 )));
    _materials[material]->setReflectionIndex(0.8f);
    _materials[material]->setSpecularColor( WHITE );
    _materials[material]->setSpecularExponent( 10.f );

    // Lamp
    ++material;
    const Vector3f lampInfo = { 0.15f, 0.99f, 0.15f };
    const Vector3f lampPositions[4] =
    {
        { 0.5f - lampInfo.x(), lampInfo.y(), 0.5f - lampInfo.z() },
        { 0.5f + lampInfo.x(), lampInfo.y(), 0.5f - lampInfo.z() },
        { 0.5f + lampInfo.x(), lampInfo.y(), 0.5f + lampInfo.z() },
        { 0.5f - lampInfo.x(), lampInfo.y(), 0.5f + lampInfo.z() }
    };
    for( size_t i = 0; i < 4; ++i )
        _trianglesMeshes[material].getVertices().push_back( lampPositions[i] );
    _trianglesMeshes[material].getIndices().push_back( Vector3i( 2, 1, 0 ));
    _trianglesMeshes[material].getIndices().push_back( Vector3i( 0, 3, 2 ));
    _materials[material]->setColor( WHITE );
    _materials[material]->setEmission( 5.f );

    BRAYNS_INFO << "Bounding Box: " << _bounds << std::endl;
}

void Scene::buildEnvironment( )
{
    switch( _parametersManager.getGeometryParameters().getSceneEnvironment( ) )
    {
    case SceneEnvironment::none:
        break;
    case SceneEnvironment::ground:
    {
        // Ground
        const Vector3f scale( 4.f, 0.5f, 4.f );
        const float tiles = 8.f;
        const float S = 0.5f * std::min(
            _bounds.getSize( ).x( ), _bounds.getSize( ).z( ));
        const Vector3f s( S,_bounds.getSize( ).y( ), S );
        const Vector3f c = _bounds.getCenter( );

        Vector3i i;
        const size_t material = 0;
        size_t meshIndex = _trianglesMeshes[material].getIndices( ).size( );

        Vector4f v;
        const Vector4f n( 0.f, 1.f, 0.f, 0.f );
        v = Vector4f(
            c.x( ) - s.x( ) * scale.x( ),
            c.y( ) - s.y( ) * scale.y( ) * 1.001f,
            c.z( ) - s.z( ) * scale.z( ),
            0.f);
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( 0.f, 0.f ));
        v = Vector4f(
            c.x( ) + s.x( ) * scale.x( ),
            c.y( ) - s.y( ) * scale.y( ) * 1.001f,
            c.z( ) - s.z( ) * scale.z( ),
            0.f);
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( tiles, 0.f ));
        v = Vector4f(
            c.x( ) + s.x( ) * scale.x( ),
            c.y( ) - s.y( ) * scale.y( ) * 1.001f,
            c.z( ) + s.z( ) * scale.z( ),
            0.f);
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( tiles, tiles ));
        i = Vector3i( meshIndex, meshIndex+1, meshIndex+2 );
        _trianglesMeshes[material].getIndices( ).push_back( i );
        meshIndex += 3;

        v = Vector4f(
            c.x( ) + s.x( ) * scale.x( ),
            c.y( ) - s.y( ) * scale.y( ) * 1.001f,
            c.z( ) + s.z( ) * scale.z( ),
            0.f );
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( tiles, tiles ));
        v = Vector4f(
            c.x( ) - s.x( ) * scale.x( ),
            c.y( ) - s.y( ) * scale.y( ) * 1.001f,
            c.z( ) + s.z( ) * scale.z( ),
            0.f );
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( 0.f, tiles ));
        v = Vector4f(
            c.x( ) - s.x( ) * scale.x( ),
            c.y( ) - s.y( ) * scale.y( ) * 1.001f,
            c.z( ) - s.z( ) * scale.z( ),
            0.f );
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( 0.f, 0.f ));
        i = Vector3i( meshIndex, meshIndex+1, meshIndex+2 );
        _trianglesMeshes[material].getIndices( ).push_back( i );
        break;
    }
    case SceneEnvironment::wall:
    {
        // Wall
        const Vector3f scale( 1.f, 1.f, 1.f );
        const float tiles = 4.f;
        const float S = 0.5f * std::min(
            _bounds.getSize( ).x( ), _bounds.getSize( ).z( ));
        const Vector3f s( S, _bounds.getSize( ).y( ), S );
        const Vector3f c = _bounds.getCenter( );
        Vector3i i;
        const size_t material = 0;
        size_t meshIndex = _trianglesMeshes[material].getIndices( ).size( );
        Vector4f v;
        const Vector4f n( 0.f, 0.f, -1.f, 0.f );
        v = Vector4f(
            c.x( ) - s.x( ) * scale.x( ),
            c.y( ) - s.y( ) * scale.y( ),
            c.z( ) + s.z( ) * scale.z( ) * 1.001f,
            0.f);
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( 0.f, 0.f ));
        v = Vector4f(
            c.x( ) + s.x( ) * scale.x( ),
            c.y( ) - s.y( ) * scale.y( ),
            c.z( ) + s.z( ) * scale.z( ) * 1.001f,
            0.f);
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( tiles, 0.f ));
        v = Vector4f(
            c.x( ) + s.x( ) * scale.x( ),
            c.y( ) + s.y( ) * scale.y( ),
            c.z( ) + s.z( ) * scale.z( ) * 1.001f,
            0.f );
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( tiles, tiles ));
        i = Vector3i( meshIndex, meshIndex+1, meshIndex+2 );
        _trianglesMeshes[material].getIndices( ).push_back( i );
        meshIndex += 3;
        v = Vector4f(
            c.x( ) + s.x( ) * scale.x( ),
            c.y( ) + s.y( ) * scale.y( ),
            c.z( ) + s.z( ) * scale.z( ) * 1.001f,
            0.f );
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( tiles, tiles ));
        v = Vector4f(
            c.x( ) - s.x( ) * scale.x( ),
            c.y( ) + s.y( ) * scale.y( ),
            c.z( ) + s.z( ) * scale.z( ) * 1.001f,
            0.f );
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( 0.f, tiles ));
        v = Vector4f(
            c.x( ) - s.x( ) * scale.x( ),
            c.y( ) - s.y( ) * scale.y( ),
            c.z( ) + s.z( ) * scale.z( ) * 1.001f,
            0.f);
        _trianglesMeshes[material].getVertices( ).push_back( v );
        _trianglesMeshes[material].getNormals( ).push_back( n );
        _trianglesMeshes[material].getTextureCoordinates( ).push_back(
            Vector2f( 0.f, 0.f ));
        i = Vector3i( meshIndex, meshIndex+1, meshIndex+2 );
        _trianglesMeshes[material].getIndices( ).push_back( i );
        break;
    }
    case SceneEnvironment::bounding_box:
    {
        const size_t material = MATERIAL_BOUNDING_BOX;
        const Vector3f s = _bounds.getSize()/2.f;
        const Vector3f c = _bounds.getCenter();
        const float radius = s.length()/500.f;
        const Vector3f positions[8] =
        {
            { c.x() - s.x(), c.y() - s.y(), c.z() - s.z() },
            { c.x() + s.x(), c.y() - s.y(), c.z() - s.z() }, //    6--------7
            { c.x() - s.x(), c.y() + s.y(), c.z() - s.z() }, //   /|       /|
            { c.x() + s.x(), c.y() + s.y(), c.z() - s.z() }, //  2--------3 |
            { c.x() - s.x(), c.y() - s.y(), c.z() + s.z() }, //  | |      | |
            { c.x() + s.x(), c.y() - s.y(), c.z() + s.z() }, //  | 4------|-5
            { c.x() - s.x(), c.y() + s.y(), c.z() + s.z() }, //  |/       |/
            { c.x() + s.x(), c.y() + s.y(), c.z() + s.z() }  //  0--------1
        };

        for( size_t i = 0; i < 8; ++i)
            _primitives[material].push_back( SpherePtr(
                new Sphere( material, positions[i], radius, 0, 0)));

        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[0], positions[1], radius, 0, 0)));
        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[2], positions[3], radius, 0, 0)));
        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[4], positions[5], radius, 0, 0)));
        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[6], positions[7], radius, 0, 0)));

        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[0], positions[2], radius, 0, 0)));
        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[1], positions[3], radius, 0, 0)));
        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[4], positions[6], radius, 0, 0)));
        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[5], positions[7], radius, 0, 0)));

        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[0], positions[4], radius, 0, 0)));
        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[1], positions[5], radius, 0, 0)));
        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[2], positions[6], radius, 0, 0)));
        _primitives[material].push_back( CylinderPtr(
            new Cylinder( material, positions[3], positions[7], radius, 0, 0)));

        break;
    }
    }
}

void Scene::addLight( LightPtr light )
{
    removeLight( light );
    _lights.push_back( light );
}

void Scene::removeLight( LightPtr light )
{
    Lights::iterator it = std::find( _lights.begin( ), _lights.end( ), light );
    if( it != _lights.end( ))
        _lights.erase( it );
}

LightPtr Scene::getLight( const size_t index )
{
    if( index < _lights.size() )
        return _lights[index];
    return 0;
}

void Scene::clearLights( )
{
    _lights.clear();
}

void Scene::setSimulationHandler( AbstractSimulationHandlerPtr handler )
{
    _simulationHandler = handler;
}

AbstractSimulationHandlerPtr Scene::getSimulationHandler() const
{
    return _simulationHandler;
}

VolumeHandlerPtr Scene::getVolumeHandler()
{
    const auto& volumeFile = _parametersManager.getVolumeParameters().getFilename();
    const auto& volumeFolder = _parametersManager.getVolumeParameters().getFolder();
    if( volumeFile.empty() && volumeFolder.empty() )
        return nullptr;

    try
    {
        if( !_volumeHandler )
        {
            _volumeHandler.reset( new VolumeHandler(
                _parametersManager.getVolumeParameters(), TimestampMode::modulo ));
            if( !volumeFile.empty() )
                _volumeHandler->attachVolumeToFile( 0.f, volumeFile );
            else
            {
                strings filenames;

                boost::filesystem::directory_iterator endIter;
                if( boost::filesystem::is_directory(volumeFolder))
                {
                    for( boost::filesystem::directory_iterator dirIter( volumeFolder );
                         dirIter != endIter; ++dirIter )
                    {
                        if( boost::filesystem::is_regular_file(dirIter->status( )))
                        {
                            const std::string& filename = dirIter->path( ).string( );
                            filenames.push_back( filename );
                        }
                    }
                }
                std::sort(filenames.begin(), filenames.end());
                float timestamp = 0.f;
                for( const auto& filename: filenames )
                {
                    _volumeHandler->attachVolumeToFile( timestamp, filename );
                    timestamp += 1.f;
                }
            }
        }
    }
    catch( const std::runtime_error& e )
    {
        BRAYNS_ERROR << e.what() << std::endl;
    }

    return _volumeHandler;
}

bool Scene::empty() const
{
    return _primitives.empty() && _trianglesMeshes.empty();
}

}
