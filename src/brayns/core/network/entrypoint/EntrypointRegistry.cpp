/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "EntrypointRegistry.h"

#include <stdexcept>

#include <fmt/format.h>

namespace brayns
{
EntrypointRef *EntrypointRegistry::find(const std::string &method)
{
    auto i = _entrypoints.find(method);
    return i == _entrypoints.end() ? nullptr : &i->second;
}

void EntrypointRegistry::add(EntrypointRef entrypoint)
{
    auto &method = entrypoint.getMethod();
    if (method.empty())
    {
        throw std::invalid_argument("Entrypoints must have a method name");
    }
    if (find(method))
    {
        throw std::invalid_argument(fmt::format("Entrypoint '{}' already registered", method));
    }
    _entrypoints.emplace(method, std::move(entrypoint));
}
} // namespace brayns
