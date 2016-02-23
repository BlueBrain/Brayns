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

#include "Scene.h"

#include <brayns/common/log.h>
#include <brayns/common/parameters/GeometryParameters.h>
#include <brayns/common/material/Material.h>
#include <plugins/loaders/MorphologyLoader.h>
#include <plugins/loaders/ProteinLoader.h>

#include <boost/filesystem.hpp>

#ifdef BRAYNS_USE_ASSIMP
#  include <plugins/loaders/MeshLoader.h>
#endif

namespace brayns
{

Scene::Scene( RendererPtr renderer, GeometryParameters& geometryParameters )
    : _geometryParameters( geometryParameters )
    , _renderer( renderer )
    , _timestamp( std::numeric_limits< float >::max( ))
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
        switch( materialType )
        {
        case MT_DEFAULT:
            material->setColor( Vector3f(
                float( std::rand( ) % 255 ) / 255.f,
                float( std::rand( ) % 255 ) / 255.f,
                float( std::rand( ) % 255 ) / 255.f ));
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

void Scene::loadSWCFolder( )
{
    const boost::filesystem::path& folder = _geometryParameters.getSWCFolder( );
    BRAYNS_INFO << "Loading SWC morphologies from " << folder << std::endl;
    MorphologyLoader morphologyLoader( _geometryParameters );

    size_t fileIndex = 0;
    boost::filesystem::directory_iterator endIter;
    if( boost::filesystem::exists(folder) &&
        boost::filesystem::is_directory(folder))
    {
        for( boost::filesystem::directory_iterator dirIter( folder );
             dirIter != endIter; ++dirIter )
        {
            if( boost::filesystem::is_regular_file(dirIter->status( )))
            {
                boost::filesystem::path fileExtension =
                    dirIter->path( ).extension( );
                if( fileExtension==".swc")
                {
                    const std::string& filename = dirIter->path( ).string( );
                    BRAYNS_INFO << "- " << filename << std::endl;
                    Vector3f position(0.f,0.f,0.f);
                    if( !morphologyLoader.importMorphologies(
                         MFF_SWC, filename, ++fileIndex,
                         position, _primitives, _bounds ))
                    {
                        BRAYNS_ERROR << "Failed to import " <<
                            filename << std::endl;
                    }
                }
            }
        }
    }
}

void Scene::loadPDBFolder( )
{
    // Load PDB Folder
    const boost::filesystem::path& folder = _geometryParameters.getPDBFolder( );
    BRAYNS_INFO << "Loading PDB files from " << folder << std::endl;
    ProteinLoader proteinLoader(_geometryParameters);
    if( !proteinLoader.importPDBFolder(
        0, _materials, true, _primitives, _bounds ))
    {
        BRAYNS_ERROR << "Failed to import " << folder << std::endl;
    }

    for( size_t i = 0; i < _materials.size( ); ++i )
    {
        float r,g,b;
        proteinLoader.getMaterialKd( i, r, g, b );
        MaterialPtr material = getMaterial( i );
        material->setColor( Vector3f( r, g, b ));
    }
}

void Scene::loadH5Folder( )
{
#ifdef BRAYNS_USE_BBPSDK
    // Load h5 files
    const std::string& folder = _geometryParameters.getH5Folder( );
    BRAYNS_INFO << "Loading H5 morphologies from " << folder << std::endl;
    const Vector3f position( 0.f, 0.f, 0.f );
    MorphologyLoader morphologyLoader(_geometryParameters);
    if( !morphologyLoader.importMorphologies( MFF_SWC, folder, 0, position,
         _primitives, _bounds))
        BRAYNS_ERROR << "Failed to import folder " << folder << std::endl;
#endif
}

void Scene::loadMeshFolder( )
{
#ifdef BRAYNS_USE_ASSIMP
    const boost::filesystem::path& folder =
        _geometryParameters.getMeshFolder( );
    BRAYNS_INFO << "Loading meshes from " << folder << std::endl;
    MeshLoader meshLoader;
    size_t meshIndex = 0;
    const float scale = 1.f;

    boost::filesystem::directory_iterator endIter;
    if( boost::filesystem::exists(folder) &&
        boost::filesystem::is_directory(folder))
    {
        for( boost::filesystem::directory_iterator dirIter( folder );
             dirIter != endIter; ++dirIter )
        {
            if( boost::filesystem::is_regular_file(dirIter->status( )))
            {
                boost::filesystem::path fileExtension =
                    dirIter->path( ).extension( );
                if( fileExtension==".obj" || fileExtension==".ply" ||
                    fileExtension==".dae" || fileExtension==".3ds" )
                {
                    const std::string& filename = dirIter->path( ).string( );
                    BRAYNS_INFO << "- " << filename << std::endl;
                    if(!meshLoader.importMeshFromFile(
                        filename, scale, _trianglesMeshes, _materials,
                        _geometryParameters.getColorScheme( ) == CS_NEURON_BY_ID
                        ? meshIndex : NO_MATERIAL, MQ_FAST, _bounds))
                    {
                        BRAYNS_ERROR << "Failed to import " <<
                            filename << std::endl;
                    }
                    ++meshIndex;
                }
            }
        }
    }
#endif
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
            _bounds.getDimension( ).x( ), _bounds.getDimension( ).z( ));
        const Vector3f s( S,_bounds.getDimension( ).y( ), S );
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
            _bounds.getDimension( ).x( ), _bounds.getDimension( ).z( ));
        const Vector3f s( S, _bounds.getDimension( ).y( ), S );
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

void Scene::clearLights( )
{
    _lights.clear();
}

}
