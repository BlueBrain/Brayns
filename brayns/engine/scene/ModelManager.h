/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/model/Model.h>
#include <brayns/engine/model/ModelInstance.h>
#include <brayns/utils/IDFactory.h>

#include <memory>
#include <unordered_set>

namespace brayns
{
/**
 * @brief The SceneModelManager class manages models within a scene
 */
class ModelManager
{
public:
    /**
     * @brief Adds a model to the list and creates an instance out of it to be rendered.
     * @param model
     * @return ModelInstance*
     */
    ModelInstance *add(std::shared_ptr<brayns::Model> model);

    /**
     * @brief Adds a list of new model to the scene and creates instances out of them to be rendered.
     * Will automatically trigger the scene to re-compute the bounds.
     * @param models The model to add to the scene
     * @return std::vector<ModelInstance *>
     */
    std::vector<ModelInstance *> add(std::vector<std::shared_ptr<Model>> models);

    /**
     * @brief Creates a new instance from the model that is being instantiated by the given instance ID
     * @param sourceInstanceId The ID of the instance whose model will be used to create a new instance
     * @returns ModelInstance &
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    ModelInstance *createInstance(const uint32_t sourceInstanceId);

    /**
     * @brief Returns the model instance identified by the given instance ID
     * @returns ModelInstance &
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    ModelInstance &getModelInstance(const uint32_t instanceID);

    /**
     * @brief Return a list of all model instances in the manager
     * @return std::vector<ModelInstance *> &
     */
    const std::vector<std::unique_ptr<ModelInstance>> &getAllModelInstances() const noexcept;

    /**
     * @brief Removes all model instances from the scene, identified by the given instance IDs.
     * If the model to which the instance refers does not have any other instance, it will be deleted as well.
     * Will automatically trigger the scene to re-compute the bounds.
     * @param instanceIDs list of IDs of instances to remove
     */
    void removeModelInstances(const std::vector<uint32_t> &instanceIDs);

    /**
     * @brief Clear all models and instances.
     *
     */
    void removeAllModelInstances();

private:
    friend class Scene;

    /**
     * @brief Calls the preRender system of all models
     * @param parameters
     */
    void preRender(const ParametersManager &parameters);

    /**
     * @brief Attempts to commit any unsynced data to Ospray.
     * @return CommitResult information about the result of the commit
     */
    CommitResult commit();

    /**
     * @brief Returns an aggregate of all instance bounds
     * @return Bounds
     */
    Bounds getBounds() const noexcept;

    /**
     * @brief Return a list with all the ospray instance handles
     * @return std::vector<ospray::cpp::Instance>
     */
    std::vector<ospray::cpp::Instance> getHandles() noexcept;

private:
    IDFactory<uint32_t> _instanceIdFactory;
    std::vector<std::unique_ptr<ModelInstance>> _instances;
    bool _dirty = false;
};
}
