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

#include <brayns/engine/Camera.h>
#include <brayns/engine/EngineObject.h>
#include <brayns/engine/Light.h>
#include <brayns/engine/Material.h>
#include <brayns/engine/Renderer.h>

#include <brayns/json/Json.h>

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace brayns
{
template<typename T>
class EngineObjectFactory
{
public:
    struct AbstractItemCreator
    {
        using Ptr = std::unique_ptr<AbstractItemCreator>;

        virtual std::unique_ptr<T> deserialize(const JsonValue &value) const = 0;
        virtual JsonValue serialize(const T& object) const noexcept = 0;
    };

    template<typename SubT>
    struct ItemCreator final : public AbstractItemCreator
    {
        std::unique_ptr<T> deserialize(const JsonValue& value)  const noexcept override
        {
            return Json::deserialize<std::unique_ptr<SubT>>(value);
        }

        JsonValue serialize(const T& object) const noexcept
        {
            const SubT& casted = static_cast<const SubT&>(object);
            return Json::serialize<SubT>(casted);
        }
    };

public:
    using Ptr = std::shared_ptr<EngineObjectFactory<T>>;

    template<typename SubT>
    void registerType() noexcept
    {
        const auto name = EngineObjectName<SubT>::get();
        _creators[std::string(name)] = std::make_unique<ItemCreator<SubT>>();
    }

    std::unique_ptr<T> deserialize(std::string_view name, const JsonValue& params = JsonValue()) const
    {
        auto& creator = _findCreator(name);
        return creator.deserialize(params);
    }

    std::unique_ptr<T> instantiate(std::string_view type) const
    {
        return deserialize(type);
    }

    JsonValue serialize(const T& object) const
    {
        auto& creator = _findCreator(object.getName());
        return creator.serialize(object);
    }

    std::vector<std::string> getAllTypeNames() const noexcept
    {
        std::vector<std::string> result;
        result.reserve(_creators.size());
        for(const auto& [name, creator] : _creators)
        {
            result.push_back(name);
        }
        return result;
    }
private:
    auto& _findCreator(std::string_view name)
    {
        auto it = _creators.find(name);
        if(it == _creators.end())
        {
            throw std::invalid_argument("Unknown type " + std::string(name));
        }

        auto& creator = *(it->second);
        return creator;
    }

private:
    std::map<std::string, typename AbstractItemCreator::Ptr, std::less<>> _creators;
};

using CameraFactory = EngineObjectFactory<Camera>;
using LightFactory = EngineObjectFactory<Light>;
using MaterialFactory = EngineObjectFactory<Material>;
using RendererFactory = EngineObjectFactory<Renderer>;
}
