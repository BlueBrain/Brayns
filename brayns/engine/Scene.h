/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/BaseObject.h>
#include <brayns/common/MaterialsColorMap.h>
#include <brayns/common/scene/ClipPlane.h>
#include <brayns/common/transferFunction/TransferFunction.h>
#include <brayns/engine/LightManager.h>
#include <brayns/engine/Model.h>
#include <brayns/parameters/AnimationParameters.h>
#include <brayns/parameters/VolumeParameters.h>

#include <shared_mutex>

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
    virtual void commit();

    /**
     * Commits lights to renderers.
     * @return True if lights were committed, false otherwise
     */
    virtual bool commitLights() = 0;

    /** Factory method to create an engine-specific model. */
    virtual ModelPtr createModel() const = 0;

    //@}

    /**
     * Creates a scene object responsible for handling models, simulations and
     * light sources.
     */
    Scene(AnimationParameters& animationParameters,
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
    size_t addModel(ModelDescriptorPtr model);

    /**
     * @brief Adds a model to the scene with the specific model id
     * @param id to be used as model identifier
     * @param model the model itself
     */
    void addModel(const size_t id, ModelDescriptorPtr model);

    /**
        Removes a model from the scene
        @param id id of the model (descriptor)
        @return True if model was found and removed, false otherwise
      */
    bool removeModel(const size_t id);

    /**
     * @brief Check wether a model has been marked for replacement
     *        for a new model
     * @param id the ID of the model to replace the current one
     * @return true if the model is on the list to be replaced
     */
    bool isMarkedForReplacement(const size_t id);

    /**
     * @brief Replaces an existing model (given its ID) for a new one
     * @param id the ID of the model to replace
     * @param modelDescriptor The model which will replace the current one
     * @return True if the model was found and replace, false otherwise
     */
    bool replaceModel(const size_t id, ModelDescriptorPtr modelDescriptor);

    ModelDescriptorPtr getModel(const size_t id) const;

    const std::vector<ModelDescriptorPtr>& getModels() const
    {
        return _modelDescriptors;
    }

    /**
        Builds a default scene made of a Cornell box, a reflective cube, and
        a transparent sphere
    */
    void buildDefault();

    /**
     * @return true if the scene does not contain any geometry, false otherwise
     */
    bool empty() const;

    /** Add a clip plane to the scene.
     * @param plane The coefficients of the clip plane equation.
     * @return The clip plane ID.
     */
    size_t addClipPlane(const Plane& plane);

    /** Get a clip plane by its ID.
        @param id the plane ID.
        @return A pointer to the clip plane or null if not found.
     */
    ClipPlanePtr getClipPlane(const size_t id) const;

    /** Remove a clip plane by its ID, or nop if not found. */
    void removeClipPlane(const size_t id);

    /**
       @return the clip planes
    */
    const std::vector<ClipPlanePtr>& getClipPlanes() const
    {
        return _clipPlanes;
    }
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

    MaterialPtr getBackgroundMaterial() const { return _backgroundMaterial; }

    /** @return the transfer function used for volumes and simulations. */
    TransferFunction& getTransferFunction() { return _transferFunction; }
    /** @return the transfer function used for volumes and simulations. */
    const TransferFunction& getTransferFunction() const
    {
        return _transferFunction;
    }

    /**
     * Adds the list of models to the scene
     *
     * @param input list of models to add to the scene
     * @param params Parameters for the model to be loaded
     * @throws std::runtime_error if any of the models being added is not
     * correct
     */
    void addModels(std::vector<ModelDescriptorPtr>& input,
                   const ModelParams& params);

    void visitModels(const std::function<void(Model&)>& functor);

    /** @internal */
    auto acquireReadAccess() const
    {
        return std::shared_lock<std::shared_timed_mutex>(_modelMutex);
    }

    /** @internal */
    void copyFrom(const Scene& rhs);

    virtual void copyFromImpl(const Scene&) {}

protected:
    /** @return True if this scene supports scene updates from any thread. */
    virtual bool supportsConcurrentSceneUpdates() const { return false; }
    void _computeBounds();
    void _updateAnimationParameters();

    AnimationParameters& _animationParameters;
    VolumeParameters& _volumeParameters;
    MaterialPtr _backgroundMaterial;

    TransferFunction _transferFunction;

    // Model
    size_t _modelID{0};
    std::vector<ModelDescriptorPtr> _modelDescriptors;
    mutable std::shared_timed_mutex _modelMutex;

    std::unordered_map<size_t, ModelDescriptorPtr> _markedForReplacement;

    LightManager _lightManager;
    std::vector<ClipPlanePtr> _clipPlanes;

    Boxd _bounds;
};

using ScenePtr = std::shared_ptr<Scene>;

} // namespace brayns
