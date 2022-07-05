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
#include <brayns/utils/StringUtils.h>

namespace brayns
{
struct EngineObjectData
{
    std::string name;
    JsonValue params;
};

/**
 * @brief Utility class to deserialize object sub types based on their names
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
                const auto errorMessage = string_utils::join(validationResult, "\n");
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
    std::unique_ptr<T> create(const EngineObjectData &payload) const
    {
        auto &name = payload.name;
        auto begin = _items.begin();
        auto end = _items.end();
        auto it = std::find_if(begin, end, [&](const auto &entry) { return entry->getName() == name; });

        if (it == end)
        {
            throw std::invalid_argument("Unknown type name " + name);
        }

        const auto &creator = *it;

        return creator->create(payload.params);
    }

    std::unique_ptr<T> createOr(const EngineObjectData &payload, const T &defaultValue) const
    {
        auto &name = payload.name;
        if (name.empty())
        {
            return defaultValue.clone();
        }

        return create(payload);
    }

    template<typename SubT>
    void registerType()
    {
        auto name = SubT().getName();
        _items.push_back(std::make_unique<FactoryEntry<SubT>>(name));
    }

private:
    std::vector<std::unique_ptr<IFactoryEntry>> _items;
};
}
