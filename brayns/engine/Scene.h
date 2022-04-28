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
#include <brayns/engine/scenecomponents/SceneClipManager.h>
#include <brayns/engine/scenecomponents/SceneLightManager.h>
#include <brayns/engine/scenecomponents/SceneModelManager.h>
#include <brayns/parameters/ParametersManager.h>

#include <ospray/ospray.h>

namespace brayns
{
/**
 * @brief The Scene class is the container of objects that are being rendered. It contains Models, which can
 * be geometry, volumes or clipping geometry, as well as the lights.
 */
class Scene
{
public:
    Scene();
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
     * @brief commit implementation.
     * @return True if anything changed since the last commit operation, false otherwise
     */
    bool commit();

    /**
     * @brief Get the scene model manager object
     *
     * @return SceneModelManager&
     */
    SceneModelManager &getModelManager() noexcept;

    /**
     * @brief Get the scene model manager object
     *
     * @return const SceneModelManager&
     */
    const SceneModelManager &getModelManager() const noexcept;

    /**
     * @brief Get the scene clipping models manager object
     *
     * @return SceneClipperManager&
     */
    SceneClipManager &getClipManager() noexcept;

    /**
     * @brief Get the scene light manager object
     *
     * @return SceneLightManager&
     */
    SceneLightManager &getLightManager() noexcept;

    /**
     * @brief Returns the OSPRay handle of the scene
     */
    OSPWorld handle() const noexcept;

private:
    friend class Engine;

    // Scene bounds
    Bounds _bounds;

    // Model data
    SceneModelManager _modelManager;

    // Clipping model data
    SceneClipManager _clippingManager;

    // Lights data
    SceneLightManager _lightManager;

    // OSPRRay "scene" handle
    OSPWorld _handle{nullptr};
};

} // namespace brayns
