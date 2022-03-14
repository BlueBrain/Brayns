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
#include <brayns/common/Transformation.h>
#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/ModelGroup.h>
#include <brayns/parameters/ParametersManager.h>

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
        component.onStart();
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
     * @brief Returns the OSPRay group handler object
     */
    ModelGroup &getGroup() noexcept;

private:
    /**
     * @brief Compute the model bounds, taking into account the given trasnformation
     * @param transform Matrix with the trasnformation to apply to the model data
     * @return Bounds of the model
     */
    Bounds computeBounds(const Matrix4f &transform) const noexcept;

    /**
     * @brief Subclasses should override it to provide a more accurate measure of the memory footprint.
     */
    size_t getSizeInBytes() const noexcept;

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
    friend class Scene;
    friend class ModelInstance;

private:
    // The model index refers to the position of this model in the Model list of the scene.
    uint32_t _modelIndex{};
    std::map<std::string, std::string> _metadata;
    ModelComponentContainer _components;
    ModelGroup _group;
};

/**
 * @brief The ModelInstance class is a wrapper around a Model. It shares the data provided by the Model
 * with other instances, and applies a custom trasnformation and visibility.
 */
class ModelInstance
{
public:
    /**
     * @brief Initializes the instance with the unique ID and the given model
     */
    ModelInstance(const size_t modelID, Model &model);

    ModelInstance(const ModelInstance &) = delete;
    ModelInstance &operator=(const ModelInstance &) = delete;

    ModelInstance(ModelInstance &&) = delete;
    ModelInstance &operator=(ModelInstance &&) = delete;

    ~ModelInstance();

    /**
     * @brief Returns this instance ID
     */
    uint32_t getID() const noexcept;

    /**
     * @brief Returns the bounds of this instance in world space coordinates
     */
    const Bounds &getBounds() const noexcept;

    /**
     * @brief Recompute the model bounds with the current transformation
     */
    void computeBounds() noexcept;

    /**
     * @brief Commit implementation
     */
    bool commit();

    /**
     * @brief Returns a mutable version of the model this instance refers to.
     */
    Model &getModel() noexcept;

    /**
     * @brief Returns a const reference to the model this instance refers to.
     */
    const Model &getModel() const noexcept;

    /**
     * @brief Utility function to return this Model Instance underlying model metadata
     */
    const std::map<std::string, std::string> &getModelMetadata() const noexcept;

    /**
     * @brief Sets wether this instance is visible or not.
     */
    void setVisible(const bool val) noexcept;

    /**
     * @brief Returns wether this instance is visible or not.
     */
    bool isVisible() const noexcept;

    /**
     * @brief Sets the transformation of this instance.
     */
    void setTransform(const Transformation &transform) noexcept;

    /**
     * @brief Returns the trasnsformation of this instance.
     */
    const Transformation &getTransform() const noexcept;

private:
    /**
     * @brief Returns the OSPRay handle of this instance.
     */
    OSPInstance handle() const noexcept;

private:
    friend class Scene;

private:
    const uint32_t _modelID{};
    Model &_model;

    bool _visible{true};
    bool _visibilityChanged {true};
    Transformation _transformation;
    Bounds _bounds;

    OSPInstance _instanceHandle{nullptr};
};
} // namespace brayns
