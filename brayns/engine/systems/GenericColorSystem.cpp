/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "GenericColorSystem.h"

#include <algorithm>

#include <spdlog/fmt/fmt.h>

namespace
{
class MethodFinder
{
public:
    static auto &get(const std::vector<std::unique_ptr<brayns::IColorMethod>> &methods, const std::string &name)
    {
        auto begin = methods.begin();
        auto end = methods.end();
        auto it = std::find_if(begin, end, [&](auto &handler) { return handler->getName() == name; });

        if (it == end)
        {
            throw std::invalid_argument(fmt::format("Color method '{}' not found", name));
        }

        return **it;
    }
};
}

namespace brayns
{
GenericColorSystem::GenericColorSystem(std::vector<std::unique_ptr<IColorMethod>> methods)
    : _methods(std::move(methods))
{
}

std::vector<std::string> GenericColorSystem::getMethods() const
{
    auto result = std::vector<std::string>();
    result.reserve(_methods.size());

    for (auto &method : _methods)
    {
        result.push_back(method->getName());
    }

    return result;
}

std::vector<std::string> GenericColorSystem::getValues(const std::string &method, Components &components) const
{
    auto &handler = MethodFinder::get(_methods, method);
    return handler.getValues(components);
}

void GenericColorSystem::apply(const std::string &method, const ColorMethodInput &input, Components &components) const
{
    auto &handler = MethodFinder::get(_methods, method);
    handler.apply(components, input);
}
}
