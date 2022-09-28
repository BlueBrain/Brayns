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

#include "Camera.h"

#include <brayns/engine/common/MathTypesOsprayTraits.h>

namespace
{
struct CameraParameters
{
    inline static const std::string position = "position";
    inline static const std::string direction = "direction";
    inline static const std::string up = "up";
    inline static const std::string aspect = "aspect";
};
}

namespace brayns
{
Camera::Camera(Camera &&other) noexcept
{
    *this = std::move(other);
}

Camera &Camera::operator=(Camera &&other) noexcept
{
    _projectionName = std::move(other._projectionName);
    _handle = std::move(other._handle);
    _data = std::move(other._data);
    _view = other._view;
    _aspectRatio = other._aspectRatio;
    _flag = std::move(other._flag);
    return *this;
}

Camera::Camera(const Camera &other)
{
    *this = other;
}

Camera &Camera::operator=(const Camera &other)
{
    _projectionName = other._projectionName;
    _handle = ospray::cpp::Camera(_projectionName);
    _data = other._data->clone();
    _data->pushTo(_handle);
    setView(other._view);
    setAspectRatio(other._aspectRatio);
    _flag.setModified(true);
    return *this;
}

const std::string &Camera::getName() const noexcept
{
    return _projectionName;
}

void Camera::setView(const View &view)
{
    _flag.update(_view, view);
}

const View &Camera::getView() const noexcept
{
    return _view;
}

void Camera::setAspectRatio(float aspectRatio)
{
    _flag.update(_aspectRatio, aspectRatio);
}

bool Camera::commit()
{
    if (!_flag)
    {
        return false;
    }
    _data->pushTo(_handle);
    _updateView();
    _updateAspectRatio();
    _handle.commit();
    _flag = false;
    return true;
}

const ospray::cpp::Camera &Camera::getHandle() const noexcept
{
    return _handle;
}

void Camera::_updateView()
{
    auto &position = _view.position;
    auto &target = _view.target;
    auto &up = _view.up;
    auto forward = glm::normalize(target - position);
    auto strafe = glm::cross(forward, up);
    auto realUp = glm::cross(strafe, forward);

    _handle.setParam(CameraParameters::position, position);
    _handle.setParam(CameraParameters::direction, forward);
    _handle.setParam(CameraParameters::up, realUp);
}

void Camera::_updateAspectRatio()
{
    _handle.setParam(CameraParameters::aspect, _aspectRatio);
}
}
