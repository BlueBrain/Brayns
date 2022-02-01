/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#include "Statistics.h"

namespace brayns
{
double Statistics::getFPS() const
{
    return _fps;
}

void Statistics::setFPS(const double fps)
{
    _updateValue(_fps, fps);
}

size_t Statistics::getSceneSizeInBytes() const
{
    return _sceneSizeInBytes;
}

void Statistics::setSceneSizeInBytes(const size_t sceneSizeInBytes)
{
    _updateValue(_sceneSizeInBytes, sceneSizeInBytes);
}
} // namespace brayns
