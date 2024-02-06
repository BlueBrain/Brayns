/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include <brayns/engine/components/Bounds.h>
#include <brayns/parameters/ParametersManager.h>

#include "ModelManager.h"

#include <ospray/ospray_cpp/World.h>

namespace brayns
{
/**
 * @brief The Scene class is the container of objects that are being rendered. It contains Models, which can
 * be geometry, volumes or clipping geometry, as well as the lights.
 */
class Scene
{
public:
    Scene() = default;

    Scene(const Scene &) = delete;
    Scene &operator=(const Scene &) = delete;

    Scene(Scene &&) = delete;
    Scene &operator=(Scene &&) = delete;

    /**
     * @brief Return the bounds of the scene
     */
    Bounds getBounds() const noexcept;

    /**
     * @brief Called before a new frame is rendered to update all models.
     */
    void update(const ParametersManager &params);

    /**
     * @brief commit implementation.
     * @return True if anything changed since the last commit operation, false otherwise
     */
    bool commit();

    /**
     * @brief Returns the model manager of the scene
     * @return ModelManager&
     */
    ModelManager &getModels() noexcept;

    /**
     * @copydoc Scene::getModels() noexcept
     * @return const ModelManager&
     */
    const ModelManager &getModels() const noexcept;

    /**
     * @brief Returns the Ospray handle of the scene
     */
    const ospray::cpp::World &getHandle() const noexcept;

private:
    friend class Engine;

    Bounds _bounds;
    ModelManager _models;
    ospray::cpp::World _handle;
};

} // namespace brayns
