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
#include <brayns/common/simulation/AbstractSimulationHandler.h>
#include <brayns/common/transferFunction/TransferFunction.h>
#include <brayns/common/types.h>

SERIALIZATION_ACCESS(Scene)

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
        Returns the bounding box of the scene
    */
    Boxf getBounds();
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
        Adds a model to the scene
      */
    BRAYNS_API virtual ModelPtr addModel(
        const std::string& name, const ModelMetadata& metadata = {}) = 0;

    /**
        Removes a geometry group from the scene
        @param index Index of the model
      */
    BRAYNS_API virtual void removeModel(const size_t index) = 0;

    /**registry
        Builds a default scene made of a Cornell box, a reflective cube, and
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
    BRAYNS_API VolumeHandlerPtr getVolumeHandler();

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
     * @internal needed to ensure deletion wrt cyclic dependency
     *           scene<->renderer
     */
    virtual void reset();

    /** @return the current size in bytes of the loaded geometry. */
    size_t getSizeInBytes() const { return _sizeInBytes; }
    ModelDescriptors& getModelDescriptors() { return _modelDescriptors; }
    /**
     * @brief initializeMaterials Initializes materials for all models in the
     * scene
     * @param colorMap Color map to use for every individual model
     */
    void setMaterialsColorMap(
        MaterialsColorMap colorMap = MaterialsColorMap::none);

    MaterialPtr getBackgroundMaterial() { return _backgroundMaterial; }
protected:
    Renderers _renderers;
    ParametersManager& _parametersManager;
    MaterialPtr _backgroundMaterial;

    // Model
    ModelDescriptors _modelDescriptors;
    bool _geometryGroupsDirty{true};
    Lights _lights;

    // Volume
    VolumeHandlerPtr _volumeHandler{nullptr};

    // Simulation
    AbstractSimulationHandlerPtr _simulationHandler{nullptr};
    TransferFunction _transferFunction;
    CADiffusionSimulationHandlerPtr _caDiffusionSimulationHandler{nullptr};

    size_t _sizeInBytes{0};

private:
    void _markGeometryDirty();

    SERIALIZATION_FRIEND(Scene)
};
}
#endif // SCENE_H
