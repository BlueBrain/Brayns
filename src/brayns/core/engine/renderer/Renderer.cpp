/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "Renderer.h"

namespace brayns
{
Renderer::Renderer(Renderer &&other) noexcept
{
    *this = std::move(other);
}

Renderer &Renderer::operator=(Renderer &&other) noexcept
{
    _handleName = std::move(other._handleName);
    _rendererName = std::move(other._rendererName);
    _handle = std::move(other._handle);
    _data = std::move(other._data);
    _flag = std::move(other._flag);
    return *this;
}

Renderer::Renderer(const Renderer &other)
{
    *this = other;
}

Renderer &Renderer::operator=(const Renderer &other)
{
    _handleName = other._handleName;
    _rendererName = other._rendererName;
    _handle = ospray::cpp::Renderer(_handleName);
    _data = other._data->clone();
    _data->pushTo(_handle);
    _flag.setModified(true);
    return *this;
}

bool Renderer::commit()
{
    if (!_flag)
    {
        return false;
    }

    _flag = false;
    _handle.commit();
    return true;
}

size_t Renderer::getSamplesPerPixel() const noexcept
{
    return _data->getSamplesPerPixel();
}

const std::string &Renderer::getName() const noexcept
{
    return _rendererName;
}

const ospray::cpp::Renderer &Renderer::getHandle() const noexcept
{
    return _handle;
}
}
