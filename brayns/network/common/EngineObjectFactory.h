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

#include <brayns/json/Json.h>
#include <brayns/json/JsonSchemaValidator.h>

#include <brayns/network/adapters/CameraAdapter.h>
#include <brayns/network/adapters/RendererAdapter.h>

#include <brayns/utils/string/StringJoiner.h>

namespace brayns
{
/**
 * @brief Utility class to deserialize object sub types based on their names
 *
 * @tparam T Abstract type
 */
template<typename T>
class EngineObjectFactory
{
public:
    class IFactoryEntry
    {
    public:
        virtual ~IFactoryEntry() = default;

        virtual std::unique_ptr<T> create(const JsonValue &data) const = 0;

        virtual std::string getName() const noexcept = 0;
    };

    template<typename SubT>
    class FactoryEntry final : public IFactoryEntry
    {
    public:
        FactoryEntry(std::string entryName)
            : _name(std::move(entryName))
        {
        }

        std::unique_ptr<T> create(const JsonValue &data) const override
        {
            const auto schema = Json::getSchema<SubT>();

            const auto validationResult = JsonSchemaValidator::validate(data, schema);

            if (!validationResult.empty())
            {
                const auto errorMessage = StringJoiner::join(validationResult, "\n");
                throw std::invalid_argument("Cannot deserialize type " + _name + ":" + errorMessage);
            }

            auto result = std::make_unique<SubT>();
            Json::deserialize<SubT>(data, *result);
            return result;
        }

        std::string getName() const noexcept override
        {
            return _name;
        }

    private:
        std::string _name;
    };

public:
    std::unique_ptr<T> create(const std::string &name, const JsonValue &data) const
    {
        const auto begin = _items.begin();
        const auto end = _items.end();
        auto it = std::find_if(
            begin,
            end,
            [&](const std::unique_ptr<IFactoryEntry> &entry) { return entry->getName() == name; });

        if (it == end)
        {
            throw std::invalid_argument("Unknown type name " + name);
        }

        const auto &creator = *it;

        return creator->create(data);
    }

    template<typename SubT>
    void registerType()
    {
        auto temp = std::make_unique<SubT>();
        auto name = temp->getName();
        _items.push_back(std::make_unique<FactoryEntry<SubT>>(name));
    }

private:
    std::vector<std::unique_ptr<IFactoryEntry>> _items;
};

template<typename T>
class GenericObject
{
public:
    GenericObject() = default;
    GenericObject(T &object, EngineObjectFactory<T> &factory)
        : _systemObject(&object)
        , _factory(&factory)
        , _value(Json::parse("{}"))
    {
    }

    void setName(const std::string &name)
    {
        _name = name;
    }

    void setValue(const JsonValue &value)
    {
        _value = value;
    }

    std::unique_ptr<T> create()
    {
        if (_name.empty())
        {
            return _systemObject->clone();
        }

        return _factory->create(_name, _value);
    }

private:
    T *_systemObject{nullptr};
    EngineObjectFactory<T> *_factory{nullptr};
    std::string _name;
    JsonValue _value;
};

#define GENERIC_OBJECT_ADAPTER(TYPE) \
    BRAYNS_JSON_ADAPTER_BEGIN(GenericObject<TYPE>) \
    BRAYNS_JSON_ADAPTER_SET("name", setName, "Type name", Required(false)) \
    BRAYNS_JSON_ADAPTER_SET("params", setValue, "Type parameters", Required(false)) \
    BRAYNS_JSON_ADAPTER_END()

GENERIC_OBJECT_ADAPTER(Camera)
GENERIC_OBJECT_ADAPTER(Renderer)

struct EngineFactories
{
    static EngineObjectFactory<Camera> createCameraFactory() noexcept;

    static EngineObjectFactory<Renderer> createRendererFactory() noexcept;
};
}
