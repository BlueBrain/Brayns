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

#pragma once

#include <brayns/common/Log.h>

#include <map>
#include <memory>
#include <string_view>
#include <typeindex>

namespace
{
struct IComponent
{
    using Ptr = std::unique_ptr<IComponent>;

    virtual ~IComponent() = default;
};

template<typename T>
struct Component : public IComponent
{
    Component(T& src)
     : value(&src)
    {
    }

    T* value {nullptr};
};

template<typename T, typename Map>
decltype(auto) extract(Map& map, std::string_view key)
{
    auto it = map.find(key);

    if(it == map.end())
    {
        brayns::Log::error("Unknown component binding key '{}'", key);
        throw std::invalid_argument("Wrong binding key");
    }

    try
    {
        auto& castedWrapper = dynamic_cast<Component<T>&>(*(it->second));
        return *castedWrapper.value;
    }
    catch (const std::bad_cast& bc)
    {
        brayns::Log::error("Could not cast component binding");
        throw bc;
    }
}
}

namespace brayns
{
class ModelComponents
{
public:
    template<typename T>
    void bindComponent(std::string_view key, T& component)
    {
        _components[std::string(key)] = std::make_unique<Component<T>>(component);
    }

    template<typename T>
    T& getComponent(std::string_view key)
    {
        return extract<T>(_components, key);
    }

    template<typename T>
    const T& getComponent(std::string_view key) const
    {
        return extract<T>(_components, key);
    }

private:
    std::map<std::string, IComponent::Ptr, std::less<>> _components;
};
}
