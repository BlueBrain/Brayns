/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#ifndef SCENE_H
#define SCENE_H

#include <brayns/api.h>
#include <brayns/common/BaseObject.h>
#include <brayns/common/geometry/Cone.h>
#include <brayns/common/geometry/Cylinder.h>
#include <brayns/common/geometry/Sphere.h>
#include <brayns/common/geometry/TrianglesMesh.h>
#include <brayns/common/material/Material.h>
#include <brayns/common/material/Texture2D.h>
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/transferFunction/TransferFunction.h>
#include <brayns/common/types.h>

namespace brayns
{
/**

   Scene object

   This object contains collections of geometries, materials and light sources
   that are used to describe the 3D scene to be rendered. Scene is the base
   class for rendering-engine-specific inherited scenes.
 */
class Scene : public BaseObject
{
public:
    /**
        Creates a scene object responsible for handling geometry, volumes,
       materials and
        light sources.
        @param renderers Renderers to be used to render the scene
        @param parametersManager Parameters for the scene (Geometry, volume,
       rendering, etc)
        @todo The scene must not know about the renderer
              https://bbpteam.epfl.ch/project/issues/browse/VIZTM-574
    */
    BRAYNS_API Scene(Renderers renderers, ParametersManager& parametersManager);

    BRAYNS_API virtual ~Scene();

    /**
     * Called after scene-related changes have been made before rendering the
     * scene.
     */
    BRAYNS_API virtual void commit() = 0;

    /**
        Commit lights to renderers
    */
    BRAYNS_API virtual void commitLights() = 0;

    /**
        Converts scene geometry into rendering engine specific data structures
    */
    BRAYNS_API virtual void buildGeometry() = 0;

    /**
        Serializes scene geometry into rendering engine specific data structures
    */
    BRAYNS_API virtual void serializeGeometry() = 0;

    /**
        Attach simulation data to renderer
    */
    BRAYNS_API virtual void commitSimulationData() = 0;

    /**
        Attach volume data to renderer
    */
    BRAYNS_API virtual void commitVolumeData() = 0;

    /**
        Commit transfer function data to renderer
    */
    BRAYNS_API virtual void commitTransferFunctionData() = 0;

    /**
        Returns the bounding box for the whole scene
    */
    Boxf& getWorldBounds() { return _bounds; }
    const Boxf& getWorldBounds() const { return _bounds; }
    /**
        Build an environment in addition to the loaded data, and according to
        the geometry parameters (command line parameter --scene-environment).
    */
    BRAYNS_API void buildEnvironment();

    /**
        Attaches a light source to the scene
        @param light Object representing the light source
    */
    BRAYNS_API void addLight(LightPtr light);

    /**
        Gets a light source from the scene for a given index
        @return Pointer to light source
    */
    BRAYNS_API LightPtr getLight(const size_t index);

    /**
        Removes a light source from the scene for a given index
        @param light Light source to be removed
    */
    BRAYNS_API void removeLight(LightPtr light);

    /**
        Removes all light sources from the scene
    */
    BRAYNS_API void clearLights();

    /**
        Builds a default scene made of a Cornell box, a refelctive cube, and
        a transparent sphere
    */
    BRAYNS_API void buildDefault();

    /**
        Return true if the scene does not contain any geometry. False otherwise
    */
    BRAYNS_API bool empty() const;

    BRAYNS_API ParametersManager& getParametersManager()
    {
        return _parametersManager;
    }

    /**
        Returns spheres handled by the scene
    */
    BRAYNS_API SpheresMap& getSpheres() { return _spheres; }
    /**
        Returns cylinders handled by the scene
    */
    BRAYNS_API CylindersMap& getCylinders() { return _cylinders; }
    /**
        Returns cones handled by the scene
    */
    BRAYNS_API ConesMap& getCones() { return _cones; }
    /**
        Returns textures handled by the scene
    */
    BRAYNS_API TexturesMap& getTextures() { return _textures; }
    /**
        Returns triangle meshes handled by the scene
    */
    BRAYNS_API TrianglesMeshMap& getTriangleMeshes()
    {
        return _trianglesMeshes;
    }

    /**
        Returns the simulutation handler
    */
    BRAYNS_API AbstractSimulationHandlerPtr getSimulationHandler() const;

    /**
        Sets the simulation handler
    */
    BRAYNS_API void setSimulationHandler(AbstractSimulationHandlerPtr handler);

    /**
        Returns volume data
    */
    BRAYNS_API VolumeHandlerPtr getVolumeHandler() const;

    /**
        Sets the Calcium diffusion simulation handler
    */
    void setCADiffusionSimulationHandler(
        CADiffusionSimulationHandlerPtr handler);

    /**
        Gets the Calcium diffusion simulation handler
    */
    CADiffusionSimulationHandlerPtr getCADiffusionSimulationHandler() const;

    /**
        Build a color map from a file, according to the colormap-file scene
       parameters
    */
    BRAYNS_API TransferFunction& getTransferFunction()
    {
        return _transferFunction;
    }

    /** Unloads geometry, materials, lights, models, etc. to free memory. */
    BRAYNS_API virtual void unload();

    /** Loads geometry a binary cache file defined by the --load-cache-file
       command line parameter. The cache file is a binary representation of the
       following structure:
       - Version
       - Number of materials
       - Materials
       - For each material:
         - Number of spheres
         - Spheres
         - Number of cylinders
         - Cylinders
         - Number of cones
         - Cones
         - Number of vertices
         - Vertices
         - Number of indices
         - Indices
         - Number of normals
         - Normals
         - Number of texture coordinates
         - Texture coordinates
       - Scene bounds
    */
    BRAYNS_API void loadFromCacheFile();

    /**
        Saves geometry a binary cache file defined by the --save-cache-file
       command line parameter. See loadFromCacheFile for file structure
    */
    BRAYNS_API void saveToCacheFile();

    /**
     * @return true if the given volume file is supported by the engines' scene.
     *         If false, a default scene will be constructed.
     */
    BRAYNS_API virtual bool isVolumeSupported(
        const std::string& volumeFile) const = 0;

    /**
     * @brief Sets spheres as dirty, meaning that they need to be serialized
     *        and sent to the rendering engine
     */
    BRAYNS_API void setSpheresDirty(const bool value) { _spheresDirty = value; }
    /**
     * @internal needed to ensure deletion wrt cyclic dependency
     *           scene<->renderer
     */
    virtual void reset();

    /**
      Adds a sphere to the scene
      @param materialId Material of the sphere
      @param sphere Sphere to add
      @return Index of the sphere for the specified material
      */
    BRAYNS_API uint64_t addSphere(const size_t materialId,
                                  const Sphere& sphere);

    /**
      Adds a cone to the scene
      @param materialId Material of the cone
      @param cone Cone to add
      @return Index of the sphere for the specified material
      */
    BRAYNS_API uint64_t addCone(const size_t materialId, const Cone& cone);

    /**
      Adds a cylinder to the scene
      @param materialId Material of the cylinder
      @param cylinder Cylinder to add
      @return Index of the sphere for the specified material
      */
    BRAYNS_API uint64_t addCylinder(const size_t materialId,
                                    const Cylinder& cylinder);

    /**
      Replaces a sphere in the scene
      @param materialId Material of the sphere
      @param index Index of the sphere in the scene, for the given material
      @param sphere New sphere

      */
    BRAYNS_API void setSphere(const size_t materialId, const uint64_t index,
                              const Sphere& sphere);

    /**
      Replaces a cone in the scene
      @param materialId Material of the cone
      @param index Index of the cone in the scene, for the given material
      @param cone New sphere

      */
    BRAYNS_API void setCone(const size_t materialId, const uint64_t index,
                            const Cone& cone);

    /**
      Replaces a cylinder in the scene
      @param materialId Material of the cylinder
      @param index Index of the cylinder in the scene, for the given material
      @param cylinder New cylinder

      */
    BRAYNS_API void setCylinder(const size_t materialId, const uint64_t index,
                                const Cylinder& cylinder);

    /**
      Adds a material to the scene
      @param material Material to add
      @return index of the new material
      */
    BRAYNS_API size_t addMaterial(const Material& material);

    /**
        Returns the material object for a given index
        @return Reference to material object
    */
    BRAYNS_API Material& getMaterial(size_t index);

    /**
        Set the material object for a given index
        @param index Index of material in the scene
        @param material Material object
    */
    BRAYNS_API void setMaterial(const size_t index, const Material& material);

    /**
        Resets materials and builds the system ones (Bounding box, skymap, etc)
    */
    BRAYNS_API void resetMaterials();

    /**
        Commit materials to renderers
        @param action Defines if materials should be created or updated
    */
    BRAYNS_API virtual void commitMaterials(
        const Action action = Action::create) = 0;

    /**
        Returns materials handled by the scene
    */
    BRAYNS_API Materials& getMaterials() { return _materials; }
    /**
        Sets the materials handled by the scene, and available to the
        scene geometry
        @param colorMap Specifies the algorithm that is used to create
               the materials. For instance MT_RANDOM creates materials with
               random colors, transparency, reflection, and light emission
    */
    BRAYNS_API void setMaterialsColorMap(
        MaterialsColorMap colorMap = MaterialsColorMap::none);

    /** @return the current size in bytes of the loaded geometry. */
    size_t getSizeInBytes() const { return _sizeInBytes; }
protected:
    void _buildMissingMaterials(const size_t materialId);
    void _processVolumeAABBGeometry();

    // Parameters
    ParametersManager& _parametersManager;
    Renderers _renderers;

    // Model
    SpheresMap _spheres;
    bool _spheresDirty;
    CylindersMap _cylinders;
    bool _cylindersDirty;
    ConesMap _cones;
    bool _conesDirty;
    TrianglesMeshMap _trianglesMeshes;
    bool _trianglesMeshesDirty;
    Materials _materials;
    TexturesMap _textures;
    Lights _lights;

    // Volume
    mutable VolumeHandlerPtr _volumeHandler;

    // Simulation
    AbstractSimulationHandlerPtr _simulationHandler;
    TransferFunction _transferFunction;
    CADiffusionSimulationHandlerPtr _caDiffusionSimulationHandler;

    // Scene
    Boxf _bounds;

    size_t _sizeInBytes{0};

private:
    void _markGeometryDirty();
};
}
#endif // SCENE_H
