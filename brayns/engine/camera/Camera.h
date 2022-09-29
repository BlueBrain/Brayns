/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/common/ModifiedFlag.h>
#include <brayns/engine/common/DataWrapper.h>

#include "ProjectionTraits.h"
#include "View.h"

#include <ospray/ospray_cpp/Camera.h>

#include <memory>

namespace brayns
{
class Camera
{
private:
    template<typename ProjectionType>
    using Data = DataWrapper<ProjectionType, ospray::cpp::Camera, ProjectionTraits>;

public:
    template<typename T>
    using Traits = ProjectionTraits<T>;

    template<typename ProjectionType>
    Camera(ProjectionType data)
    {
        set(std::move(data));
    }

    Camera(Camera &&) noexcept;
    Camera &operator=(Camera &&) noexcept;

    Camera(const Camera &);
    Camera &operator=(const Camera &);

    /**
     * @brief Tries to cast the projection data to the given type.
     * @tparam ProjectionType Projection type to cast the data to
     * @return const ProjectionType*, will be nullptr if the cast fails
     */
    template<typename ProjectionType>
    const ProjectionType *as() const noexcept
    {
        if (auto cast = dynamic_cast<const Data<ProjectionType> *>(_data.get()))
        {
            return &cast->data;
        }
        return nullptr;
    }

    /**
     * @brief Sets the camera projection and recreates the OSPRay handle to accomodate it.
     * @tparam ProjectionType Type of projection to set
     * @param projection Projection data
     */
    template<typename ProjectionType>
    void set(ProjectionType data) noexcept
    {
        _projectionName = ProjectionTraits<ProjectionType>::name;
        _handle = ospray::cpp::Camera(_projectionName);
        _data = std::make_unique<Data<ProjectionType>>(std::move(data));
        _flag = true;
    }

    /**
     * @brief Returns the projection name.
     * @return const std::string&.
     */
    const std::string &getName() const noexcept;

    /**
     * @brief Sets the camera view.
     * @param view (position, target, up direction).
     */
    void setView(const View &view);

    /**
     * @brief Returns the camera view.
     * @return const View& (position, target, up direction).
     */
    const View &getView() const noexcept;

    /**
     * @brief Sets the render plane aspect ratio.
     * @param aspectRatio (width/height).
     */
    void setAspectRatio(float aspectRatio);

    /**
     * @brief Calls the underlying OSPRay commit function, if any parameter has been modified, and resets the modified
     * state.
     * @return true If any parameter was modified and thus the commit function was called.
     * @return false If no parameter was modified.
     */
    bool commit();

    /**
     * @brief Returns the OSPRay camera handle.
     * @return const ospray::cpp::Camera&.
     */
    const ospray::cpp::Camera &getHandle() const noexcept;

private:
    void _updateView();
    void _updateAspectRatio();

private:
    std::string _projectionName;
    ospray::cpp::Camera _handle;
    std::unique_ptr<IDataWrapper<ospray::cpp::Camera>> _data;
    View _view;
    float _aspectRatio = 1.f;
    ModifiedFlag _flag;
};
}
