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

#include "Components.h"
#include "Systems.h"
#include "SystemsView.h"

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
    explicit Model(std::string type);

    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    Model(Model &&) = delete;
    Model &operator=(Model &&) = delete;

    /**
     * @brief Returns the model ID
     * @return uint32_t
     */
    uint32_t getID() const noexcept;

    /**
     * @brief Returns the type of model.
     * @return const std::string&
     */
    const std::string &getType() const noexcept;

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
     * @brief Returns a view to the systems
     * @return SystemsView
     */
    SystemsView getSystemsView() noexcept;

private:
    /**
     * @brief Called when the model is added to the scene
     */
    void init();

    /**
     * @brief Commits the model data to OSPRay
     * @return CommitResult Required actions after model commit.
     */
    CommitResult commit();

private:
    friend class ModelManager;

    uint32_t _id{};
    std::string _type;
    ospray::cpp::Group _handle;

    Components _components;
    Systems _systems;
};
} // namespace brayns
