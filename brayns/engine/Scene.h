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
#include <vector>

namespace brayns
{

/**
 * @brief The ModelsLoadParameters struct holds the information with which a group of models was loaded
 */
struct ModelsLoadParameters
{
    std::string type; // "Binary" or "File"
    std::string path;
    std::string givenName;
    std::string loaderName;
    JsonValue loadParameters;
};

class Scene : public EngineObject
{
public:
    Scene();
    ~Scene();

    const Bounds &getBounds() const noexcept;

    std::vector<ModelInstance*> addModels(ModelsLoadParameters params, std::vector<Model::Ptr>&& models);
    ModelInstance& getModel(const uint32_t modelID);
    const ModelInstance& getModel(const uint32_t modelID) const;
    std::vector<uint32_t> getAllModelIDs() const noexcept;
    void removeModel(const uint32_t modelID);

    uint32_t addLight(Light::Ptr&& light) noexcept;
    const Light& getLight(const uint32_t lightID) const;
    void removeLight(const uint32_t lightID);

    void commit() final;

    OSPWorld handle() const noexcept;

private:
    struct ModelIndex
    {
        // used to find and delete a ModelIndex when the model it corresponds with is deleted from the scene
        uint32_t modelId {};
        // Index in the array of loaded models spitted by the loader
        uint32_t modelIndex {};
    };
    /**
     * @brief The ModelsLoadEntry struct is used to keep track of every add-model request
     * made to the system. It is used when creating a "dump image" of the current system so that
     * it can be restored later.
     *
     * This, however, does not allow to dump binary-uploaded models. To allow for that, the
     * dump image method should be implemented as a binary file with the binary contents of the
     * system, which could become a 300 GB+ file.
     *
     * For this reason. the current implementation simply dumps the state of the objects of the
     * engine + the parameters to load the active models
     */
    struct ModelsLoadEntry
    {
        ModelsLoadParameters params;
        // For a given loader + parameters, it will produce the same models, in the same
        // order, on every call. modelIndices holds the indices of the models which must be
        // kept after performing the loader call. This is because an user might have loaded
        // multiple models, but then deleted some before creating the dump.
        std::vector<ModelIndex> modelIndices;
    };

private:
    Bounds _bounds;

    // Load is an asynchronous task, so addModels can be accessed by multiple threads
    std::mutex _loadMutex;
    uint32_t _modelIdFactory {0};
    std::map<uint32_t, ModelInstance::Ptr> _models;
    std::vector<ModelsLoadEntry> _loadEntries;

    uint32_t _lightIdFactory {0};
    std::map<uint32_t, Light::Ptr> _lights;

    OSPWorld _handle {nullptr};
};

} // namespace brayns
