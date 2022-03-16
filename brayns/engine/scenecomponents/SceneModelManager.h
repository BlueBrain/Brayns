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

#include <brayns/common/IDFactory.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/ModelInstance.h>
#include <brayns/json/JsonType.h>

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
        FROM_BLOB
    };

    LoadType type;
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
     * The model ID is returned
     */
    ModelInstance &addModel(ModelLoadParameters params, std::unique_ptr<Model> model);

    /**
     * @brief Creates a new instance from the model that is being instantiated by the given model ID,
     * adds it to the scene and returns its ID.
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    ModelInstance &createInstance(const uint32_t modelID);

    /**
     * @brief Returns the model instance identified by the given model ID
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    ModelInstance &getModelInstance(const uint32_t modelID);

    /**
     * @brief Return a list of all the IDs of the model instances on the scene
     */
    const std::vector<ModelInstance *> getAllModelInstances() const noexcept;

    /**
     * @brief Removes a model instance from the scene, identified by the given model ID.
     * If the model to which the instance refers does not have any other instance, it will be deleted as well.
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    void removeModel(const uint32_t modelID);

    /**
     * @brief Get the load parameters of the model which is referenced by the instance denoted by the given model ID
     *
     * @param modelID the model instance ID
     * @return const ModelLoadParameters
     * @throws std::invalid_argument if the given modelID does not correspond to any existing model instance
     */
    const ModelLoadParameters getModelLoadParameters(const uint32_t modelID) const;

private:
    friend class Scene;

    /**
     * @brief The ModelsLoadEntry struct is used to keep track of every model loaded into the scene and the
     * instances made out of it.
     */
    struct ModelEntry
    {
        ModelLoadParameters params;
        std::unique_ptr<Model> model;
        std::vector<std::unique_ptr<ModelInstance>> instances;
    };

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
     * @brief Creates a new instance of the given Model, adds it to the scene and returns it.
     */
    ModelInstance &_createModelInstance(ModelEntry &modelEntry);

    /**
     * @brief Return a list with all the model instance handles so that the scene can commit them
     *
     * @return std::vector<OSPInstance>
     */
    std::vector<OSPInstance> getInstanceHandles() noexcept;

private:
    IDFactory<uint32_t> _idFactory;
    std::vector<ModelEntry> _models;
    std::vector<ModelInstance *> _modelInstances;
};
}
