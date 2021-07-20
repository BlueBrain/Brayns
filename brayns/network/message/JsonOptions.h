/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#pragma once

#include "JsonSchema.h"
#include "JsonType.h"

#include <boost/optional.hpp>

namespace brayns
{
struct JsonOptions
{
    template <typename T, typename... Types>
    static void build(JsonOptions& options, T&& value, Types&&... values)
    {
        build(options, std::forward<T>(value));
        build(options, std::forward<Types>(values)...);
    }

    template <typename T>
    static void build(JsonOptions& options, T&& value)
    {
        value.add(options);
    }

    static void build(JsonOptions&) {}

    std::string title;
    std::string description;
    bool required = false;
    boost::optional<double> minimum;
    boost::optional<double> maximum;
    boost::optional<size_t> minItems;
    boost::optional<size_t> maxItems;
    JsonValue defaultValue;

    JsonOptions() = default;

    template <typename... Args>
    JsonOptions(Args&&... args)
    {
        build(*this, std::forward<Args>(args)...);
    }
};

class JsonSchemaOptions
{
public:
    static void add(JsonSchema& schema, const JsonOptions& options)
    {
        updateIfNotEmpty(schema.title, options.title);
        updateIfNotEmpty(schema.description, options.description);
        updateIfNotNull(schema.minimum, options.minimum);
        updateIfNotNull(schema.maximum, options.maximum);
        updateIfNotNull(schema.minItems, options.minItems);
        updateIfNotNull(schema.maxItems, options.maxItems);
        updateIfNotEmpty(schema.defaultValue, options.defaultValue);
    }

private:
    static void updateIfNotEmpty(JsonValue& value, const JsonValue& newValue)
    {
        if (newValue.isEmpty())
        {
            return;
        }
        value = newValue;
    }

    template <typename T>
    static void updateIfNotEmpty(T& value, const T& newValue)
    {
        if (newValue.empty())
        {
            return;
        }
        value = newValue;
    }

    template <typename T>
    static void updateIfNotNull(T& value, const T& newValue)
    {
        if (!newValue)
        {
            return;
        }
        value = newValue;
    }
};

struct Title
{
    Title(std::string value)
        : value(std::move(value))
    {
    }

    void add(JsonOptions& options) const { options.title = value; }

    std::string value;
};

struct Description
{
    Description(std::string value)
        : value(std::move(value))
    {
    }

    void add(JsonOptions& options) const { options.description = value; }

    std::string value;
};

struct Required
{
    Required() = default;

    Required(bool value)
        : value(value)
    {
    }

    void add(JsonOptions& options) const { options.required = value; }

    bool value = true;
};

struct Minimum
{
    Minimum(double value)
        : value(value)
    {
    }

    void add(JsonOptions& options) const { options.minimum = value; }

    double value = 0.0;
};

struct Maximum
{
    Maximum(double value)
        : value(value)
    {
    }

    void add(JsonOptions& options) const { options.maximum = value; }

    double value = 0.0;
};

struct MinItems
{
    MinItems(size_t value)
        : value(value)
    {
    }

    void add(JsonOptions& options) const { options.minItems = value; }

    size_t value = 0.0;
};

struct MaxItems
{
    MaxItems(size_t value)
        : value(value)
    {
    }

    void add(JsonOptions& options) const { options.maxItems = value; }

    size_t value = 0.0;
};

struct Default
{
    Default(const JsonValue& value)
        : value(value)
    {
    }

    void add(JsonOptions& options) const { options.defaultValue = value; }

    JsonValue value;
};
} // namespace brayns