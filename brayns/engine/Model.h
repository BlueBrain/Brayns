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
#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/ModelGroup.h>
#include <brayns/json/JsonType.h>

#include <ospray/ospray.h>

#include <map>
#include <memory>

namespace brayns
{
/**
 * @brief The Model class represents an isolate rendering unit in the engine. It is made up of
 * components, which adds functionality as well as renderable items, such as geometry, volumes and
 * clipping geometry.
 */
class Model
{
public:
    Model() = default;

    ~Model();

    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    Model(Model &&) = delete;
    Model &operator=(Model &&) = delete;

    /**
     * @brief Sets the metadata of this model (The metadata is model-specific information).
     * @param metadata
     */
    void setMetaData(std::map<std::string, std::string> metadata) noexcept;

    /**
     * @brief Returns the metadata of this model
     */
    const std::map<std::string, std::string> &getMetaData() const noexcept;

    /**
     * @brief Adds a new component to the model
     */
    template<typename T, typename... Args>
    T &addComponent(Args &&...args) noexcept
    {
        auto &component = _components.addComponent<T>(std::forward<Args>(args)...);
        component._owner = this;
        component.onCreate();
        return component;
    }

    /**
     * @brief Retrieves a component from the model
     */
    template<typename T>
    T &getComponent()
    {
        return _components.getComponent<T>();
    }

    template<typename T>
    std::vector<T *> getAllComponents()
    {
        return _components.getAllComponents<T>();
    }

    /**
     * @brief Removes a component from the model
     */
    template<typename T>
    void removeComponent()
    {
        _components.removeComponent<T>();
    }

    /**
     * @brief Called when a scene inspect event has hitted any geometry of this model
     */
    void onInspect(const InspectContext &context, JsonObject &writeResult) const noexcept;

    /**
     * @brief Returns the OSPRay group handler object
     */
    ModelGroup &getGroup() noexcept;

    /**
     * @brief returns the model ID
     * @return uint32_t
     */
    uint32_t getID() const noexcept;

private:
    /**
     * @brief Compute the model bounds, taking into account the given trasnformation
     * @param transform Matrix with the trasnformation to apply to the model data
     * @return Bounds of the model
     */
    Bounds computeBounds(const Matrix4f &transform) const noexcept;

    /**
     * @brief Returns the OSPRay handle
     */
    OSPGroup groupHandle() const noexcept;

    /**
     * @brief Called before the commit + rendering happens
     */
    void onPreRender(const ParametersManager &params);

    /**
     * @brief Called after the commit + rendering happens
     */
    void onPostRender(const ParametersManager &params);

    /**
     * @brief commit implementation
     */
    bool commit();

private:
    friend class SceneClipManager;
    friend class SceneModelManager;
    friend class ModelInstance;

private:
    uint32_t _modelId{};
    std::map<std::string, std::string> _metadata;
    ModelComponentContainer _components;
    ModelGroup _group;
};
} // namespace brayns
