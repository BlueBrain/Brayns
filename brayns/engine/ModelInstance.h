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
#include <brayns/engine/Model.h>

#include <ospray/ospray.h>

namespace brayns
{
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
    ModelInstance(const uint32_t modelInstanceID, Model &model);

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
    friend class SceneClipManager;
    friend class SceneModelManager;

    /**
     * @brief Returns the OSPRay handle of this instance.
     */
    OSPInstance handle() const noexcept;

    /**
     * @brief Commit implementation
     */
    bool commit(const bool modelChanged);

private:
    const uint32_t _modelInstanceID{};
    Model &_model;

    bool _visible{true};
    bool _visibilityChanged{true};
    Transformation _transformation;
    Bounds _bounds;

    OSPInstance _instanceHandle{nullptr};
};
}
