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
#include <brayns/parameters/SceneParameters.h>
#include <brayns/parameters/GeometryParameters.h>
#include <brayns/common/material/Material.h>

#include <servus/uri.h>

#include <boost/filesystem.hpp>

namespace brayns
{

Scene::Scene(
    Renderers renderers,
    SceneParameters& sceneParameters,
    GeometryParameters& geometryParameters )
    : _sceneParameters(sceneParameters)
    , _geometryParameters( geometryParameters )
    , _renderers( renderers )
{
}

Scene::~Scene( )
{
    _primitives.clear( );
    _trianglesMeshes.clear( );
}

void Scene::setMaterials(
    const MaterialType materialType,
    const size_t nbMaterials )
{
    _materials.clear( );
    for( size_t i = 0; i < nbMaterials; ++i )
    {
        MaterialPtr material( new Material );

        // Special materials (Simulation, skybox, etc)
        switch( i )
        {
            case MATERIAL_SIMULATION:
                material->setTexture( TT_DIFFUSE, TEXTURE_NAME_SIMULATION );
                break;
            default:
                break;
        }

        // Other materials
        material->setSpecularColor( Vector3f( 1.f, 1.f, 1.f ));
        material->setSpecularExponent( 10.f );

        switch( materialType )
        {
        case MT_DEFAULT:
        case MT_DIFFUSE:
        case MT_ELECTRON:
        case MT_NO_SHADING:
            switch( i )
            {
                case 1: // Soma
                    material->setColor( Vector3f( 1.f, 1.f, 1.f ));
                    break;
                case 2: // Axon
                    material->setColor( Vector3f( 0.2f, 0.2f, 0.8f ));
                    break;
                case 3: // Dendrite
                    material->setColor( Vector3f( 0.8f, 0.2f, 0.2f ));
                    break;
                case 4: // Apical dendrite
                    material->setColor( Vector3f( 0.2f, 0.2f, 0.8f ));
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

void Scene::buildEnvironment( )
{
    switch( _geometryParameters.getSceneEnvironment( ) )
    {
    case SE_NONE:
        break;
    case SE_GROUND:
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
    case SE_WALL:
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
    case SE_BOX:
    {
        BRAYNS_ERROR << "Box environment is not implemented" << std::endl;
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

}
