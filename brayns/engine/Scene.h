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
#include <brayns/engine/EngineObject.h>
#include <brayns/engine/Light.h>
#include <brayns/engine/Model.h>
#include <brayns/json/JsonType.h>

#include <ospray/ospray.h>

#include <map>
#include <mutex>
#include <set>
#include <vector>

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
 * @brief The Scene class is the container of objects that are being rendered. It contains Models, which can
 * be geometry, volumes or clipping geometry, as well as the lights.
 */
class Scene : public EngineObject
{
public:
    Scene() = default;
    ~Scene();

    Scene(const Scene&) = delete;
    Scene &operator=(const Scene&) = delete;

    Scene(Scene&&) = delete;
    Scene &operator=(Scene&&) = delete;

    /**
     * @brief Return the bounds of the scene
     */
    const Bounds &getBounds() const noexcept;

    /**
     * @brief Adds a new model to the scene and creates an instance out of it to be rendered.
     * The model ID is returned
     */
    uint32_t addModel(ModelLoadParameters params, Model::Ptr&& model);

    /**
     * @brief Creates a new instance from the model that is being instantiated by the given model ID,
     * adds it to the scene and returns its ID.
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    uint32_t createInstance(const uint32_t modelID);

    /**
     * @brief Returns the model instance identified by the given model ID
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    const ModelInstance& getModel(const uint32_t modelID) const;

    /**
     * @brief Return a list of all the IDs of the model instances on the scene
     */
    std::vector<uint32_t> getAllModelIDs() const noexcept;

    /**
     * @brief Removes a model instance from the scene, identified by the given model ID.
     * If the model to which the instance refers does not have any other instance, it will be deleted as well.
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    void removeModel(const uint32_t modelID);

    /**
     * @brief Allos to pass a callback to modify a model instance (and thus, its model) given its model ID.
     * This allows to control better when the bounds needs to be re-computed.
     * @throws std::invalid_argument if modelID does not correspond to any existing model
     */
    void manipulateModel(const uint32_t modelID, const std::function<void(ModelInstance&)>& callback);

    /**
     * @brief Adds a new model to be used as geometry and volume clipping geometry, returning the ID assigned
     * to it
     * @throws std::invalid_argument if the passed model is null
     */
    uint32_t addClippingModel(Model::Ptr&& clippingModel);

    /**
     * @brief Return a clipping model identified by its ID.
     * @throws std::invalid_argument if the given ID does not correspond to any existing clipping model
     */
    const Model& getClippingModel(const uint32_t clippingModelID) const;

    /**
     * @brief Removes a clipping plane from the scene, identified by its ID.
     * @throws std::invalid_argument if the given ID does not correspond to any existing clipping model
     */
    void removeClippingModel(const uint32_t clippingModelID);

    /**
     * @brief Adds a new light to the scene and returns the ID which identifies it.
     */
    uint32_t addLight(Light::Ptr&& light) noexcept;

    /**
     * @brief Returns the light that it is identified by the given light ID.
     * @throws std::invalid_argument if lightID does not correspond to any existing light
     */
    const Light& getLight(const uint32_t lightID) const;

    /**
     * @brief Removes the light from the scene that is identified by the given light ID
     * @throws std::invalid_argument if lightID does not correspond to any existing ligth
     */
    void removeLight(const uint32_t lightID);

    /**
     * @brief Called before a new frame is. Will call onPreRender on all the models of the scene
     */
    void preRender(const AnimationParameters& animation);

    /**
     * @brief Called after a new frame is rendered. Will call onPostRender on all the models of the scene
     */
    void postRender();

    /**
     * @brief commit implementation. Will call the doCommit() implementation of the models and the lights.
     */
    void commit() final;

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

    /**
     * @brief Creates a new instance of the given Model, adds it to the scene and returns it.
     */
    ModelInstance& _createModelInstance(Model* model);

    /**
     * @brief Recompute bounds based on its current instances
     */
    void _recomputeBounds() noexcept;

private:
    /**
     * @brief The ModelsLoadEntry struct is used to keep track of every model loaded into the scene and the
     * instances made out of it.
     */
    struct ModelEntry
    {
        ModelLoadParameters params;
        Model::Ptr model {nullptr};
        std::set<uint32_t> instances;
    };

private:
    // Scene bounds
    Bounds _bounds;

    // Model data
    uint32_t _modelIdFactory {0};
    std::vector<ModelEntry> _models; // Stores loaded models
    std::map<uint32_t, ModelInstance::Ptr> _modelInstances; // Stores model instances

    // Clipping model data
    uint32_t _clippingModelIDFactory {0};
    std::map<uint32_t, Model::Ptr> _clippingModels;

    // Lights data
    uint32_t _lightIdFactory {0};
    std::map<uint32_t, Light::Ptr> _lights;

    // OSPRRay "scene" handle
    OSPWorld _handle {nullptr};
};

} // namespace brayns
