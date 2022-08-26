/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <brayns/engine/camera/Camera.h>
#include <brayns/engine/light/Light.h>
#include <brayns/engine/material/Material.h>
#include <brayns/engine/renderer/Renderer.h>
#include <brayns/json/Json.h>
#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/utils/string/StringJoiner.h>

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

namespace brayns
{
struct EngineObjectData
{
    std::string name;
    JsonValue params;
};

template<typename T>
class EngineFactory
{
public:
    class IFactoryEntry
    {
    public:
        virtual ~IFactoryEntry() = default;
        virtual T deserialize() const = 0;
        virtual JsonValue serialize(const T &object) const = 0;
        virtual std::string_view getName() const noexcept = 0;
    };

    template<typename SubT>
    class FactoryEntry final : public IFactoryEntry
    {
    public:
        FactoryEntry(std::string entryName)
            : _name(std::move(entryName))
        {
        }

        T deserialize(const JsonValue &payload) const override
        {
            auto schema = Json::getSchema<SubT>();
            auto schemaErrors = JsonSchemaValidator::validate(payload, schema);
            if (!schemaErrors.empty())
            {
                auto errors = StringJoiner::join(schemaErrors, "\n");
                throw std::invalid_argument("Cannot parse json: " + errors);
            }
            auto data = Json::deserialize<SubT>(payload);
            return T(data);
        }

        JsonValue serialize(const T &object)
        {
            auto casted = object.as<SubT>();
            if (!casted)
            {
                throw std::invalid_argument("Cannot cast the object to the underlying requested type");
            }

            return Json::serialize(casted);
        }

        std::string_view getName() const noexcept override
        {
            return _name;
        }

    private:
        std::string _name;
    };

public:
    T create(const EngineObjectData &data) const
    {
        auto &name = data.name;
        auto begin = _items.begin();
        auto end = _items.end();
        auto it = std::find_if(begin, end, [&](auto &entry) { return entry->getName() == name; });
        if (it == end)
        {
            throw std::invalid_argument("Unknown type name " + name);
        }
        return (*it)->deserialize(data.params);
    }

    T createOr(const EngineObjectData &data, const T &defaultValue) const
    {
        if (data.name.empty())
        {
            return T(defaultValue);
        }
        return create(data);
    }

    template<typename SubT>
    void addType()
    {
        using Traits = T::Traits<SubT>;
        _items.push_back(std::make_unique<FactoryEntry<SubT>>(Traits::name));
    }

private:
    std::vector<std::unique_ptr<IFactoryEntry>> _items;
};

class EngineFactories
{
public:
    static EngineFactory<Camera> createCameraFactory();
    static EngineFactory<Light> createLightFactory();
    static EngineFactory<Material> createMaterialFactory();
    static EngineFactory<Renderer> createRendererFactory();
};
}
