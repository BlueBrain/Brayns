/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/common/Bounds.h>
#include <brayns/common/parameters/ParametersManager.h>

#include "ClipManager.h"
#include "LightManager.h"
#include "ModelManager.h"

#include <ospray/ospray_cpp/World.h>

namespace brayns
{
/**
 * @brief The Scene class is the container of objects that are being rendered. It contains Models, which can
 * be geometry, volumes or clipping geometry, as well as the lights.
 */
class Scene
{
public:
    Scene() = default;

    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;

    Scene(Scene &&) = delete;
    Scene &operator=(Scene &&) = delete;

    /**
     * @brief Return the bounds of the scene
     */
    const Bounds &getBounds() const noexcept;

    /**
     * @brief Recompute bounds based on its current instances and lights.
     */
    void computeBounds() noexcept;

    /**
     * @brief Called before a new frame is. Will call onPreRender on all the models of the scene
     */
    void preRender(const ParametersManager &params);

    /**
     * @brief Called after a new frame is rendered. Will call onPostRender on all the models of the scene
     */
    void postRender(const ParametersManager &params);

    /**
     * @brief commit implementation.
     * @return True if anything changed since the last commit operation, false otherwise
     */
    bool commit();

    /**
     * @brief Adds a model to the scene and returns its instance
     * @param params Parameters with which the model was loaded
     * @param model Model to add
     * @return ModelInstance&
     */
    ModelInstance &addModel(ModelLoadParameters params, std::unique_ptr<Model> model);

    /**
     * @brief Adds a list of models to the scene and returns the renderable instances made out of them.
     * @param params Parameters used to load the added models.
     * @param models List of models to add to the scene.
     * @return std::vector<ModelInstance *>
     */
    std::vector<ModelInstance *> addModels(ModelLoadParameters params, std::vector<std::unique_ptr<Model>> models);

    /**
     * @brief Removes a list of model instances by their ID.
     * @param instanceIDs List of instance IDs to remove.
     * @throws std::invalid_argument if any of the IDs does not exists in the list of instances.
     */
    void removeModelInstances(const std::vector<uint32_t> &instanceIDs);

    /**
     * @brief Removes all model instances in the scene.
     */
    void removeAllModelInstances() noexcept;

    /**
     * @brief Returns a referene to a model instance identified by its ID.
     * @param instanceId ID of the model instance to return.
     * @return ModelInstance&
     * @throw std::invalid_argument if the ID does not belong to any existing model instance.
     */
    ModelInstance &getModelInstance(uint32_t instanceId);

    /**
     * @brief Return a list of all the model instances in the scene.
     * @return const std::vector<ModelInstance *>&
     */
    const std::vector<ModelInstance *> &getAllModelInstances() const noexcept;

    /**
     * @brief Returns the parameters used to load the model used by the instance denoted in the id.
     * @param instanceId
     * @return const ModelLoadParameters
     * @throws std::invalid_argument if the ID does not belon to any existing model instance
     */
    const ModelLoadParameters &getModelLoadParameters(uint32_t instanceId) const;

    /**
     * @brief Adds a light to the scene and returns its ID/
     * @param light Light to add.
     * @return uint32_t ID of the light after adding it.
     */
    uint32_t addLight(Light light);

    /**
     * @brief Removes a list of lights from the scene identified by their ID.
     * @param lightIds
     * @throws std::invalid_argument if any of the IDs does not belong to any existing light.
     */
    void removeLights(const std::vector<uint32_t> &lightIds);

    /**
     * @brief Removes all lights in the scene.
     */
    void removeAllLights() noexcept;

    /**
     * @brief Adds a clipping model to the scene and returns its ID.
     * @param model Model to add.
     * @return uint32_t ID of the model after adding it to the scene.
     */
    uint32_t addClippingModel(std::unique_ptr<Model> model);

    /**
     * @brief Removes a list of clipping models from the scene identifid by their ID.
     * @param modelIds
     * @throw std::invalid_argument if any of the IDs does not belong to any existing clipping model.
     */
    void removeClippingModels(const std::vector<uint32_t> &modelIds);

    /**
     * @brief Removes all clipping models in the scene.
     */
    void removeAllClippingModels() noexcept;

    /**
     * @brief Returns the Ospray handle of the scene
     */
    const ospray::cpp::World &getHandle() const noexcept;

private:
    friend class Engine;

    Bounds _bounds;
    ModelManager _modelManager;
    ClipManager _clippingManager;
    LightManager _lightManager;
    ospray::cpp::World _handle;
};

} // namespace brayns
