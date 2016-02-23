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

#ifndef SCENE_H
#define SCENE_H

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/TrianglesMesh.h>

namespace brayns
{

/**

   Scene object

   This object contains collections of geometries, materials and light sources
   that are used to describe the 3D scene to be rendered. Scene is the base
   class for rendering-engine-specific inherited scenes.
 */
class Scene
{
public:
    /**
        Creates a scene object responsible for handling geometry, materials and
        light sources.
        @param renderer Renderer to be used to render the scene
        @param geometryParameters Parameters defining how the geometry is
               constructed

        @todo The scene must not know about the renderer
              https://bbpteam.epfl.ch/project/issues/browse/VIZTM-574
    */
    BRAYNS_API Scene(
        RendererPtr renderer,
        GeometryParameters& geometryParameters);
    BRAYNS_API virtual ~Scene();

    BRAYNS_API virtual void commit() = 0;

    /**
        Creates the materials handled by the scene, and available to the
        scene geometry
        @param materialType Specifies the algorithm that is used to create
               the materials. For instance MT_RANDOM creates materials with
               random colors, transparency, reflection, and light emission
        @param nbMaterials The number of materials to create
    */
    BRAYNS_API virtual void setMaterials(
        MaterialType materialType,
        size_t nbMaterials);

    /**
        Returns the material object for a given index
        @return Material object
    */
    BRAYNS_API MaterialPtr getMaterial( size_t index );

    /**
        Loads data from data source specified in the geometry parameters
    */
    BRAYNS_API virtual void loadData() = 0;

    /**
        Converts scene geometry into rendering engine specific data structures
    */
    BRAYNS_API virtual void buildGeometry() = 0;

    /**
        Returns the bounding box for the whole scene
    */
    BRAYNS_API const Boxf& getWorldBounds() const { return _bounds; }

    /**
        Loads data from SWC files located in the folder specified in the
        geometry parameters (command line parameter --swc-folder)
    */
    BRAYNS_API void loadSWCFolder();

    /**
        Loads data from PDB files located in the folder specified in the
        geometry parameters (command line parameter --pdb-folder)
    */
    BRAYNS_API void loadPDBFolder();

    /**
        Loads data from H5 files located in the folder specified in the
        geometry parameters (command line parameter --h5-folder)
    */
    BRAYNS_API void loadH5Folder();

    /**
        Loads data from mesh files located in the folder specified in the
        geometry parameters (command line parameter --mesh-folder)
    */
    BRAYNS_API void loadMeshFolder();

    /**
        Build an environment in addition to the loaded data, and according to
        the geometry parameters (command line parameter --scene-environment).
    */
    BRAYNS_API void buildEnvironment();

    /**
        Attaches a light source to the scene
        @param index Index of the light source for further use. If a light
                     source already exists for a given index, it is replaced
                     by the one
        @param light Object representing the light source
    */
    BRAYNS_API void addLight( LightPtr light );

    /**
        Removes a light source from the scene for a given index
        @param light Light source to be removed
    */
    BRAYNS_API void removeLight( LightPtr light );

    /**
        Removes all light sources from the scene
    */
    BRAYNS_API void clearLights();

    /**
        Timestamp of the scene
    */
    BRAYNS_API void setTimestamp( const float timestamp )
    {
        _timestamp = timestamp;
    }
    BRAYNS_API float getTimestamp( ) const { return _timestamp; }

protected:
    // Parameters
    GeometryParameters& _geometryParameters;
    RendererPtr _renderer;

    // Model
    PrimitivesMap _primitives;
    TrianglesMeshMap _trianglesMeshes;
    Materials _materials;
    TexturesMap _textures;
    Lights _lights;
    float _timestamp;

    Boxf _bounds;
};

}
#endif // SCENE_H
