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

#include "Components.h"
#include "RenderGroup.h"
#include "Systems.h"

#include <ospray/ospray_cpp/Group.h>

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

    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    Model(Model &&) = delete;
    Model &operator=(Model &&) = delete;

    /**
     * @brief returns the model ID
     * @return uint32_t
     */
    uint32_t getID() const noexcept;

    /**
     * @brief Returns the OSPRay group handle
     * @return ospray::cpp::Group&
     */
    ospray::cpp::Group &getHandle() noexcept;

    /**
     * @brief Returns the model's component list
     * @return Components&
     */
    Components &getComponents() noexcept;

    /**
     * @copydoc Model::getComponents() noexcept;
     */
    const Components &getComponents() const noexcept;

    /**
     * @brief Returns the model's systems manager
     * @return Systems&
     */
    Systems &getSystems() noexcept;

    /**
     * @brief Process the model if it is the target of the inspect context
     * @param context Information about the hitted model on the inspection
     * @return InspectResult the result of checking the inspection context for this model
     */
    InspectResultData inspect(const InspectContext &context);

    /**
     * @brief Compute the spatial bounds of the model
     * @param matrix Transformation to apply to the volume or geometries of the model
     * @return Bounds axis-aligned spatial bounds
     */
    Bounds computeBounds(const Matrix4f &matrix);

    /**
     * @brief Called when the model is added to the scene
     */
    void init();

    /**
     * @brief Called on the pre-render stage
     * @param parameters ParametersManager object to access system config
     */
    void onPreRender(const ParametersManager &parameters);

    /**
     * @brief Called before rendering a new frame
     * @return CommitResult Required actions given the commits made on the model
     */
    CommitResult commit();

    /**
     * @brief Called on the post-render stage
     * @param parameters ParametersManager object to access system config
     */
    void onPostRender(const ParametersManager &parameters);

private:
    friend class ModelManager;

    uint32_t _modelId{};
    ospray::cpp::Group _handle;

    Components _components;
    Systems _systems;
};
} // namespace brayns
