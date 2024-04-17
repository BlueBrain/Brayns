/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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
#include <unordered_set>

#include <fmt/format.h>

namespace
{
class MethodNameList
{
public:
    static std::vector<std::string> build(brayns::ColorMethodList &methods)
    {
        auto uniqueNames = std::unordered_set<std::string>();
        for (auto &method : methods)
        {
            auto methodName = method->getName();
            auto inserted = uniqueNames.insert(methodName);
            if (!inserted.second)
            {
                throw std::runtime_error(fmt::format("Duplicate color method name '{}'", methodName));
            }
        }

        auto result = std::vector<std::string>(uniqueNames.begin(), uniqueNames.end());
        std::sort(result.begin(), result.end());
        return result;
    }
};

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
GenericColorSystem::GenericColorSystem(ColorMethodList methods):
    _methods(std::move(methods)),
    _methodNames(MethodNameList::build(_methods))
{
}

std::vector<std::string> GenericColorSystem::getMethods() const
{
    return _methodNames;
}

std::vector<std::string> GenericColorSystem::getValues(const std::string &method, Components &components) const
{
    auto &handler = MethodFinder::get(_methods, method);
    return handler.getValues(components);
}

void GenericColorSystem::apply(const std::string &method, const ColorMethodInput &input, Components &components) const
{
    if (input.empty())
    {
        throw std::invalid_argument("Color input cannot be empty");
    }

    auto &handler = MethodFinder::get(_methods, method);
    handler.apply(components, input);
}
}
