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

#include <brayns/engine/Model.h>
#include <brayns/engine/ModelInstance.h>
#include <brayns/json/JsonType.h>
#include <brayns/utils/IDFactory.h>

#include <memory>
#include <unordered_set>

namespace brayns
{
/**
 * @brief The ModelsLoadParameters struct holds the information with which a group of models was loaded
 */
struct ModelLoadParameters
{
    enum class LoadType
    {
        FROM_FILE,
        FROM_BLOB,
        NONE,
    };

    LoadType type{LoadType::NONE};
    std::string path;
    std::string loaderName;
    JsonValue loadParameters;
};

/**
 * @brief The SceneModelManager class manages models within a scene
 */
class SceneModelManager
{
public:
    /**
     * @brief Adds a new model to the scene and creates an instance out of it to be rendered.
     * @param loadParameters Parameters which were used to load the model
     * @param model The model to add to the scene
     * @returns ModelInstance &
     */
    ModelInstance &addModel(ModelLoadParameters loadParameters, std::unique_ptr<Model> model);

    /**
     * @brief Creates a new instance from the model that is being instantiated by the given instance ID
     * @param sourceInstanceId The ID of the instance whose model will be used to create a new instance
     * @returns ModelInstance &
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    ModelInstance &createInstance(const uint32_t sourceInstanceId);

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
    std::vector<ModelInstance *> &getAllModelInstances() noexcept;

    /**
     * @brief Return a list of all model instances in the manager
     * @returns const std::vector<ModelInstance *> &
     */
    const std::vector<ModelInstance *> &getAllModelInstances() const noexcept;

    /**
     * @brief Removes a model instance from the scene, identified by the given instance ID.
     * If the model to which the instance refers does not have any other instance, it will be deleted as well.
     * @param instanceID the ID of the instance to re,pve
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    void removeModel(const uint32_t instanceID);

    /**
     * @brief Clear all models and instances.
     *
     */
    void removeAllModelInstances();

    /**
     * @brief Returns the parameters with which a model referenced by the given instance was loaded
     * @param instanceID ID of the instance that owns the model to fetch the load parameters
     * @return ModelParameters &
     * @throws std::invalid_argument if the given instanceID does not belong to any existing instance
     */
    const ModelLoadParameters &getModelLoadParameters(const uint32_t instanceID) const;

private:
    friend class Scene;

    /**
     * @brief Calls the preRender function on all components of all models
     *
     * @param parameters
     */
    void preRender(const ParametersManager &parameters);

    /**
     * @brief Attempts to commit any unsynced data to OSPRay.
     *
     * @return true if anything was commited to OSPRay, false otherwise
     */
    bool commit();

    /**
     * @brief Calls the postRender function on all componnets of all models
     *
     * @param parameters
     */
    void postRender(const ParametersManager &parameters);

    /**
     * @brief Queries the bounds of all model instances and returns a Bounds object encapsulating all of them
     *
     * @return Bounds
     */
    Bounds getBounds() const noexcept;

    /**
     * @brief Return a list with all the model instance handles so that the scene can commit them
     *
     * @return std::vector<OSPInstance>
     */
    std::vector<OSPInstance> getInstanceHandles() noexcept;

private:
    /**
     * @brief The ModelEntry struct holds all the information related to a model
     * It allows easy access to the models of the scene without having to account for instancing
     */
    struct ModelEntry
    {
        ModelLoadParameters params;
        std::unique_ptr<Model> model;
        std::vector<std::unique_ptr<ModelInstance>> instances;
    };

    /**
     * @brief Creates a new model instance from the given model entry
     * @param modelEntry ModelEntry &
     * @return ModelInstance &
     */
    ModelInstance &_createModelInstance(ModelEntry &modelEntry);

private:
    IDFactory<uint32_t> _modelIdFactory;
    IDFactory<uint32_t> _instanceIdFactory;
    std::vector<ModelEntry> _models;
    std::vector<ModelInstance *> _instances;
};
}
