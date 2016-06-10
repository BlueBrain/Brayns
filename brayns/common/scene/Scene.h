/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
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
        RendererMap renderers,
        SceneParameters& sceneParameters,
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
        Commit materials to renderers
        @param updateOnly If true, materials are not recreated and textures are
               not reassigned
    */
    BRAYNS_API virtual void commitMaterials(
        const bool updateOnly = false  ) = 0;

    /**
        Converts scene geometry into rendering engine specific data structures
    */
    BRAYNS_API virtual void buildGeometry() = 0;

    /**
        Returns the bounding box for the whole scene
    */
    BRAYNS_API Boxf& getWorldBounds() { return _bounds; }

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


    BRAYNS_API RendererMap& getRenderers() { return _renderers; }
    BRAYNS_API GeometryParameters& getGeometryParameters() { return _geometryParameters; }
    BRAYNS_API SceneParameters& getSceneParameters() { return _sceneParameters; }
    BRAYNS_API PrimitivesMap& getPrimitives() { return _primitives; }
    BRAYNS_API Materials& getMaterials() { return _materials; }
    BRAYNS_API TexturesMap& getTextures() { return _textures; }
    BRAYNS_API TrianglesMeshMap& getTriangleMeshes() { return _trianglesMeshes; }

protected:
    // Parameters
    SceneParameters& _sceneParameters;
    GeometryParameters& _geometryParameters;
    RendererMap _renderers;

    // Model
    PrimitivesMap _primitives;
    TrianglesMeshMap _trianglesMeshes;
    Materials _materials;
    TexturesMap _textures;
    Lights _lights;

    Boxf _bounds;
};

}
#endif // SCENE_H
