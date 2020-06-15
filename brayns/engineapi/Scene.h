/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/api.h>
#include <brayns/common/BaseObject.h>
#include <brayns/common/loader/LoaderRegistry.h>
#include <brayns/common/types.h>
#include <brayns/engineapi/LightManager.h>

#include <shared_mutex>

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
    /** @name API for engine-specific code */
    //@{
    /**
     * Called after scene-related changes have been made before rendering the
     * scene.
     */
    BRAYNS_API virtual void commit();

    /**
     * Commits lights to renderers.
     * @return True if lights were committed, false otherwise
     */
    BRAYNS_API virtual bool commitLights() = 0;

    /** Factory method to create an engine-specific model. */
    BRAYNS_API virtual ModelPtr createModel() const = 0;

    //@}

    /**
     * Creates a scene object responsible for handling models, simulations and
     * light sources.
     */
    BRAYNS_API Scene(AnimationParameters& animationParameters,
                     GeometryParameters& geometryParameters,
                     VolumeParameters& volumeParameters);

    /**
        Returns the bounding box of the scene
    */
    const Boxd& getBounds() const { return _bounds; }
    /** Gets the light manager */
    LightManager& getLightManager() { return _lightManager; }
    /**
        Adds a model to the scene
        @throw std::runtime_error if model is empty
      */
    BRAYNS_API size_t addModel(ModelDescriptorPtr model);

    /**
        Removes a model from the scene
        @param id id of the model (descriptor)
        @return True if model was found and removed, false otherwise
      */
    BRAYNS_API bool removeModel(const size_t id);

    BRAYNS_API const ModelDescriptors& getModelDescriptors() const
    {
        return _modelDescriptors;
    }
    BRAYNS_API ModelDescriptorPtr getModel(const size_t id) const;

    /**
        Builds a default scene made of a Cornell box, a reflective cube, and
        a transparent sphere
    */
    BRAYNS_API void buildDefault();

    /**
     * @return true if the scene does not contain any geometry, false otherwise
     */
    BRAYNS_API bool empty() const;

    /** Add a clip plane to the scene.
     * @param plane The coefficients of the clip plane equation.
     * @return The clip plane ID.
     */
    BRAYNS_API size_t addClipPlane(const Plane& plane);

    /** Get a clip plane by its ID.
        @param id the plane ID.
        @return A pointer to the clip plane or null if not found.
     */
    BRAYNS_API ClipPlanePtr getClipPlane(const size_t id) const;

    /** Remove a clip plane by its ID, or nop if not found. */
    BRAYNS_API void removeClipPlane(const size_t id);

    /**
       @return the clip planes
    */
    const ClipPlanes& getClipPlanes() const { return _clipPlanes; }
    /** @return the current size in bytes of the loaded geometry. */
    size_t getSizeInBytes() const;

    /** @return the current number of models in the scene. */
    size_t getNumModels() const;

    /**
     * @brief initializeMaterials Initializes materials for all models in the
     * scene
     * @param colorMap Color map to use for every individual model
     */
    void setMaterialsColorMap(MaterialsColorMap colorMap);

    /**
     * Set a new environment map to the scene as the background image. If envMap
     * is empty, it removes the previous map and the background color is used
     * instead.
     *
     * @param envMap a filepath to an image that shall be used as the
     *               environment map texture
     * @return false if the new map could not be set, true otherwise
     */
    bool setEnvironmentMap(const std::string& envMap);

    /**
     *  @return the current set environment map texture file, or empty if no
     *          environment is set
     */
    const std::string& getEnvironmentMap() const { return _environmentMap; }
    /** @return true if an environment map is currently set in the scene. */
    bool hasEnvironmentMap() const;

    MaterialPtr getBackgroundMaterial() const { return _backgroundMaterial; }
    /**
     * Load the model from the given blob.
     *
     * @param blob the blob containing the data to import
     * @param params Parameters for the model to be loaded
     * @param cb the callback for progress updates from the loader
     * @return the model that has been added to the scene
     */
    ModelDescriptorPtr loadModel(Blob&& blob, const ModelParams& params,
                                 LoaderProgress cb);

    /**
     * Load the model from the given file.
     *
     * @param path the file or folder containing the data to import
     * @param params Parameters for the model to be loaded
     * @param cb the callback for progress updates from the loader
     * @return the model that has been added to the scene
     */
    ModelDescriptorPtr loadModel(const std::string& path,
                                 const ModelParams& params, LoaderProgress cb);

    void visitModels(const std::function<void(Model&)>& functor);

    /** @return the registry for all supported loaders of this scene. */
    LoaderRegistry& getLoaderRegistry() { return _loaderRegistry; }
    /** @internal */
    auto acquireReadAccess() const
    {
        return std::shared_lock<std::shared_timed_mutex>(_modelMutex);
    }

    /** @internal */
    BRAYNS_API void copyFrom(const Scene& rhs);

protected:
    /** @return True if this scene supports scene updates from any thread. */
    virtual bool supportsConcurrentSceneUpdates() const { return false; }
    void _computeBounds();
    void _loadIBLMaps(const std::string& envMap);

    AnimationParameters& _animationParameters;
    GeometryParameters& _geometryParameters;
    VolumeParameters& _volumeParameters;
    MaterialPtr _backgroundMaterial;
    std::string _environmentMap;

    // Model
    size_t _modelID{0};
    ModelDescriptors _modelDescriptors;
    mutable std::shared_timed_mutex _modelMutex;

    LightManager _lightManager;
    ClipPlanes _clipPlanes;

    LoaderRegistry _loaderRegistry;
    Boxd _bounds;

private:
    SERIALIZATION_FRIEND(Scene)
};
} // namespace brayns
