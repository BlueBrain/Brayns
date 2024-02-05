/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#pragma once

#include "JsonSchema.h"
#include "JsonType.h"

#include <boost/optional.hpp>

namespace brayns
{
/**
 * @brief JSON options list for a JSON property.
 *
 * All values are optional and can be setup using a variadic list of arbitrary
 * type instances having a method add(JsonOptions&).
 *
 * Example:
 * @code {.cpp}
 * JsonOptions options = {Title("test"), Required(false), DefaultValue(3)};
 * @endcode
 *
 */
struct JsonOptions
{
    /**
     * @brief Build options using a variadic list.
     *
     * @tparam T First option type.
     * @tparam Types Other options types.
     * @param options Output options.
     * @param value First option value.
     * @param values Other option values.
     */
    template <typename T, typename... Types>
    static void build(JsonOptions& options, T&& value, Types&&... values)
    {
        build(options, std::forward<T>(value));
        build(options, std::forward<Types>(values)...);
    }

    /**
     * @brief build specialization for one option.
     *
     * @tparam T Option type.
     * @param options Output options.
     * @param value Option value.
     */
    template <typename T>
    static void build(JsonOptions& options, T&& value)
    {
        value.add(options);
    }

    /**
     * @brief build specialization for no options.
     *
     */
    static void build(JsonOptions&) {}

    /**
     * @brief Property name.
     *
     */
    boost::optional<std::string> title;

    /**
     * @brief Property description.
     *
     */
    boost::optional<std::string> description;

    /**
     * @brief Check wether the property is required.
     *
     */
    boost::optional<bool> required = false;

    /**
     * @brief Check wether the property is read only.
     *
     */
    boost::optional<bool> readOnly = false;

    /**
     * @brief Check wether the property is write only.
     *
     */
    boost::optional<bool> writeOnly = false;

    /**
     * @brief Property min value if JSON number.
     *
     */
    boost::optional<double> minimum;

    /**
     * @brief Property max value if JSON number.
     *
     */
    boost::optional<double> maximum;

    /**
     * @brief Property min item count if JSON array.
     *
     */
    boost::optional<size_t> minItems;

    /**
     * @brief Property max item count if JSON array.
     *
     */
    boost::optional<size_t> maxItems;

    /**
     * @brief Property default value.
     *
     */
    boost::optional<JsonValue> defaultValue;

    /**
     * @brief Construct an Empty option set.
     *
     */
    JsonOptions() = default;

    /**
     * @brief Construct an option set with given values.
     *
     * @tparam Args Option values types.
     * @param args Option values.
     */
    template <typename... Args>
    JsonOptions(Args&&... args)
    {
        build(*this, std::forward<Args>(args)...);
    }
};

/**
 * @brief Helper class to add options to a JSON schema.
 *
 */
class JsonSchemaOptions
{
public:
    /**
     * @brief Add the given options to the JSON schema.
     *
     * @param schema Schema to update.
     * @param options Options to set.
     */
    static void add(JsonSchema& schema, const JsonOptions& options)
    {
        trySet(schema.title, options.title);
        trySet(schema.description, options.description);
        trySet(schema.readOnly, options.readOnly);
        trySet(schema.writeOnly, options.writeOnly);
        trySet(schema.minimum, options.minimum);
        trySet(schema.maximum, options.maximum);
        trySet(schema.minItems, options.minItems);
        trySet(schema.maxItems, options.maxItems);
        trySet(schema.defaultValue, options.defaultValue);
    }

private:
    template <typename T, typename U>
    static void trySet(T& value, const U& newValue)
    {
        if (!newValue)
        {
            return;
        }
        value = *newValue;
    }
};

/**
 * @brief Add a title to a JSON option set.
 *
 */
struct Title
{
    Title(std::string title)
        : value(std::move(title))
    {
    }

    void add(JsonOptions& options) const { options.title = value; }

    std::string value;
};

/**
 * @brief Add a description to a JSON option set.
 *
 */
struct Description
{
    Description(std::string description)
        : value(std::move(description))
    {
    }

    void add(JsonOptions& options) const { options.description = value; }

    std::string value;
};

/**
 * @brief Decide wether the property is required.
 *
 */
struct Required
{
    Required() = default;

    Required(bool required)
        : value(required)
    {
    }

    void add(JsonOptions& options) const { options.required = value; }

    bool value = true;
};

/**
 * @brief Decide wether the property is read only.
 *
 */
struct ReadOnly
{
    ReadOnly() = default;

    ReadOnly(bool readOnly)
        : value(readOnly)
    {
    }

    void add(JsonOptions& options) const { options.readOnly = value; }

    bool value = true;
};

/**
 * @brief Decide wether the property is write only.
 *
 */
struct WriteOnly
{
    WriteOnly() = default;

    WriteOnly(bool writeOnly)
        : value(writeOnly)
    {
    }

    void add(JsonOptions& options) const { options.writeOnly = value; }

    bool value = true;
};

/**
 * @brief Add a minimum to an option set.
 *
 */
struct Minimum
{
    Minimum(double minimum)
        : value(minimum)
    {
    }

    void add(JsonOptions& options) const { options.minimum = value; }

    double value = 0.0;
};

/**
 * @brief Add a maximum to an option set.
 *
 */
struct Maximum
{
    Maximum(double maximum)
        : value(maximum)
    {
    }

    void add(JsonOptions& options) const { options.maximum = value; }

    double value = 0.0;
};

/**
 * @brief Add a min item count to an option set.
 *
 */
struct MinItems
{
    MinItems(size_t minItems)
        : value(minItems)
    {
    }

    void add(JsonOptions& options) const { options.minItems = value; }

    size_t value = 0.0;
};

/**
 * @brief Add a max item count to an option set.
 *
 */
struct MaxItems
{
    MaxItems(size_t maxItems)
        : value(maxItems)
    {
    }

    void add(JsonOptions& options) const { options.maxItems = value; }

    size_t value = 0.0;
};

/**
 * @brief Add a default value to an option set.
 *
 */
struct Default
{
    Default(const JsonValue& defaultValue)
        : value(defaultValue)
    {
    }

    void add(JsonOptions& options) const
    {
        options.defaultValue = value;
        options.required = false;
    }

    JsonValue value;
};
} // namespace brayns