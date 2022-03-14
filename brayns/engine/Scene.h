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
#include <brayns/common/IDFactory.h>
#include <brayns/engine/Light.h>
#include <brayns/engine/Model.h>
#include <brayns/json/JsonType.h>
#include <brayns/parameters/ParametersManager.h>

#include <ospray/ospray.h>

#include <map>
#include <mutex>
#include <set>
#include <vector>

namespace brayns
{
/**
 * @brief Handles the lights of the scene
 */
class SceneLightManager
{
public:
    struct LightEntry
    {
        uint32_t id{};
        std::unique_ptr<Light> light;
    };

public:
    /**
     * @brief Adds a new light and returns its ID
     *
     * @param light
     * @return uint32_t
     */
    uint32_t addLight(std::unique_ptr<Light> light) noexcept;

    /**
     * @brief Removes a light identifid by its ID
     * @throws std::invalid_argument if the light id does not exist
     * @param lightId
     */
    void removeLight(const uint32_t lightId);

    /**
     * @brief Removes all lights from the scene
     */
    void removeAllLigts() noexcept;

private:
    friend class Scene;

    /**
     * @brief Get the OSPRay handles of all lights
     *
     * @return std::vector<OSPLight>
     */
    std::vector<OSPLight> getLightHandles() const noexcept;

private:
    bool _modified{false};
    IDFactory<uint32_t> _idFactory;
    std::vector<LightEntry> _lights;
};

/**
 * @brief Manages all the models used to clip renderable objects in the scene
 */
class SceneClipperManager
{
public:
    struct ClippingModelEntry
    {
        uint32_t id{};
        std::unique_ptr<Model> clipper;

        ClippingModelEntry();

    private:
        static uint32_t ID_FACTORY;
    };

public:
    /**
     * @brief Adds a new clipping model
     *
     * @param clippingModel
     * @return uint32_t
     */
    uint32_t addClippingModel(std::unique_ptr<Model> clippingModel) noexcept;

    /**
     * @brief Returns a modifiable clipping model reference. Marks the manager as modified
     *
     * @param id
     * @return Model&
     */
    Model &getClippingModel(const uint32_t id);

    /**
     * @brief Get the All Clipping Models objects
     *
     * @return const std::vector<ClippingModelEntry>&
     */
    const std::vector<ClippingModelEntry> &getAllClippingModels() const noexcept;

    /**
     * @brief Removes a clipping model object
     *
     * @param id
     */
    void removeClippingModel(const uint32_t id);

    void removeAllClippingModels() noexcept;

private:
    IDFactory<uint32_t> _idFactory;
    std::vector<ClippingModelEntry> _clippingModels;
};

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
    std::vector<const ModelInstance *> getAllModelInstances() const noexcept;

    /**
     * @brief Removes a model instance from the scene, identified by the given model ID.
     * If the model to which the instance refers does not have any other instance, it will be deleted as well.
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    void removeModel(const uint32_t modelID);

private:
    /**
     * @brief Creates a new instance of the given Model, adds it to the scene and returns it.
     */
    ModelInstance &_createModelInstance(Model *model);

private:
    /**
     * @brief The ModelsLoadEntry struct is used to keep track of every model loaded into the scene and the
     * instances made out of it.
     */
    struct ModelEntry
    {
        ModelLoadParameters params;
        std::unique_ptr<Model> model;
        std::set<uint32_t> instances;
    };

    IDFactory<uint32_t> _idFactory;
    std::vector<ModelEntry> _models;
    std::vector<std::unique_ptr<ModelInstance>> _modelInstances;
};

/**
 * @brief The Scene class is the container of objects that are being rendered. It contains Models, which can
 * be geometry, volumes or clipping geometry, as well as the lights.
 */
class Scene
{
public:
    Scene() = default;
    ~Scene();

    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;

    Scene(Scene &&) = delete;
    Scene &operator=(Scene &&) = delete;

    /**
     * @brief Return the bounds of the scene
     */
    const Bounds &getBounds() const noexcept;

    /**
     * @brief Recompute bounds based on its current instances. It will use the current bounds of each model instance,
     * (it will not call computeBound() on the instances)
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
     * @brief commit implementation. Will call the doCommit() implementation of the models and the lights.
     */
    void commit();

    /**
     * @brief Returns the OSPRay handle of the scene
     */
    OSPWorld handle() const noexcept;

private:
    friend class Engine;

private:
    /**
     * @brief Returns the size in bytes of the scene
     */
    uint64_t _getSizeBytes() const noexcept;


private:
    // Scene bounds
    Bounds _bounds;

    // Model data
    SceneModelManager _modelManager;

    // Clipping model data
    SceneClipperManager _clippingManager;

    // Lights data
    SceneLightManager _lightManager;

    // OSPRRay "scene" handle
    OSPWorld _handle{nullptr};
};

} // namespace brayns
