/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "ApplicationParameters.h"

namespace brayns
{
LogLevel ApplicationParameters::getLogLevel() const noexcept
{
    return _logLevel;
}

const Vector2ui &ApplicationParameters::getWindowSize() const noexcept
{
    return _windowSize;
}

void ApplicationParameters::setWindowSize(const Vector2ui &size) noexcept
{
    _windowSize = size;
}

void ApplicationParameters::build(ArgvBuilder &builder)
{
    builder.add("log-level", _logLevel, "Log level");
    builder.add("window-size", _windowSize, "Viewport size").minimum(64);
}
} // namespace brayns
