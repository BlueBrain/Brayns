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
#include <brayns/engine/EngineObject.h>

#include <ospray/ospray.h>

#include <map>
#include <memory>

namespace brayns
{
/**
 * @brief The Model class is the base class for all models that can be loaded into Brayns.
 */
class Model : public EngineObject
{
public:
    using Ptr = std::unique_ptr<Model>;
    using Metadata = std::map<std::string, std::string>;

    /**
     * @brief Initializes the OSPRay handle
     */
    Model();

    virtual ~Model();

    /**
     * @brief onRemoved called when the model is removed from the Scene and before it is destoryed
     */
    virtual void onRemoved();

    /**
     * @brief Compute the model bounds, taking into account the given trasnformation
     * @param transform Matrix with the trasnformation to apply to the model data
     * @return Bounds of the model
     */
    virtual Bounds computeBounds(const Matrix4f& transform) const noexcept = 0;

    /**
     * @brief Sets the metadata of this model (The metadata is a map<string, string> with model-specific
     * information).
     * @param metadata
     */
    void setMetaData(Metadata metadata) noexcept;

    /**
     * @brief Returns the metadata of this model
     */
    const Metadata& getMetaData() const noexcept;

    /**
     * @brief Returns the OSPRay handle
     */
    OSPGroup groupHandle() const noexcept;

private:
    OSPGroup _groupHandle {nullptr};
    Metadata _metadata;
};

/**
 * @brief The ModelInstance class is a wrapper around a Model. It shares the data provided by the Model
 * with other instances, and applies a custom trasnformation and visibility.
 */
class ModelInstance : public EngineObject
{
public:
    using Ptr = std::unique_ptr<ModelInstance>;

    /**
     * @brief Initializes the instance with the unique ID and the given model
     */
    ModelInstance(const size_t modelID, Model* model);

    ~ModelInstance();

    /**
     * @brief Returns this instance ID
     */
    uint32_t getID() const noexcept;

    /**
     * @brief Returns the bounds of this instance in world space coordinates
     */
    const Bounds& getBounds() const noexcept;

    /**
     * @brief Commit implementation
     */
    void commit() final;

    /**
     * @brief Returns a mutable version of the model this instance refers to.
     */
    Model& getModel() noexcept;

    /**
     * @brief Returns a const reference to the model this instance refers to.
     */
    const Model& getModel() const noexcept;

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
    void setTranform(const Transformation &transform) noexcept;

    /**
     * @brief Returns the trasnsformation of this instance.
     */
    const Transformation& getTransform() const noexcept;

    /**
     * @brief Returns the OSPRay handle of this instance.
     */
    OSPInstance handle() const noexcept;

private:
    friend class Scene;

private:
    const size_t _modelID {};
    Model* _model {nullptr};

    bool _visible {true};
    Transformation _transformation;
    Bounds _bounds;

    OSPInstance _instanceHandle {nullptr};
};
} // namespace brayns
