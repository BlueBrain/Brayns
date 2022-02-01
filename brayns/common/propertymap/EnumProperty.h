/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

#include <string>
#include <vector>

#include "Converter.h"

namespace brayns
{
/**
 * @brief Generic representation of an enumeration built from its value (string
 * or index) and its available values (string list).
 *
 */
class EnumProperty
{
public:
    /**
     * @brief Construct an emtpy object with no available values.
     *
     */
    EnumProperty() = default;

    /**
     * @brief Construct an enum from its string value and available values.
     *
     * @param value Current value of the enum.
     * @param values available values of the enum.
     * @throw std::runtime_error The provided value is not in values.
     */
    EnumProperty(const std::string &value, std::vector<std::string> values);

    /**
     * @brief Construct an enum from its integer value and available values.
     *
     * @param index Current index of the value of the enum.
     * @param values available values of the enum.
     * @throw std::runtime_error The provided index is not in values.
     */
    EnumProperty(int index, std::vector<std::string> values);

    /**
     * @brief Construct an enum from its values and take the first one as
     * current value.
     *
     * @param values available values of the enum.
     */
    EnumProperty(std::vector<std::string> values);

    /**
     * @brief Get the value of the enum as a string
     *
     * @return const std::string& The enum value in available values or empty if
     * no available values.
     */
    const std::string &toString() const;

    /**
     * @brief Get the value of the enum as an integer.
     *
     * @return int The index of the current value in available values or -1 if
     * no available values.
     */
    int toInt() const;

    /**
     * @brief Get the available values of the enum.
     *
     * @return const std::vector<std::string>& The values the enum can take.
     */
    const std::vector<std::string> &getValues() const;

    /**
     * @brief Check if the given index is a valid enum value.
     *
     * @param index The index to check.
     * @return true The index is valid (>= 0 and < values.size())
     * @return false The index is invalid.
     */
    bool isValidIndex(int index) const;

    /**
     * @brief Get the index of a string in available values.
     *
     * @param value The value to get the index from values.
     * @return int The index of value in values or -1 if invalid.
     */
    int getIndex(const std::string &value) const;

    /**
     * @brief Find a value in available enum values.
     *
     * @param value The value to find.
     * @return const std::string* A pointer to the value in available values or
     * null if invalid.
     */
    const std::string *find(const std::string &value) const;

    /**
     * @brief Get the value at index in available values.
     *
     * @param value The value to find.
     * @return const std::string* A pointer to the corresponding value in
     * available values or null if invalid.
     */
    const std::string *find(int index) const;

    /**
     * @brief Assign the current enum value if valid, otherwise does nothing.
     *
     * @param value The new value of the enum.
     * @return EnumProperty& *this.
     */
    EnumProperty &operator=(const std::string &value);

    /**
     * @brief Assign the current enum value if valid, otherwise does nothing.
     *
     * @param value The index of the new value of the enum.
     * @return EnumProperty& *this.
     */
    EnumProperty &operator=(int index);

    /**
     * @brief Convert to regular enumeration from current value index.
     *
     * @tparam EnumType The type to convert the current value index.
     * @return EnumType The current index converted in EnumType.
     */
    template<typename EnumType>
    EnumType to() const
    {
        return EnumType(_index);
    }

private:
    const std::string &_getEmptyString() const;
    void _assignOrThrow(const std::string &value);
    void _assignOrThrow(int index);

    int _index = -1;
    std::vector<std::string> _values;
};

/**
 * @brief Converter to convert an enumeration to string.
 *
 */
template<>
struct Converter<EnumProperty, std::string>
{
    static void convert(const EnumProperty &from, std::string &to);
};

/**
 * @brief Converter to assign a new string value to an enumeration.
 *
 */
template<>
struct Converter<std::string, EnumProperty>
{
    static void convert(const std::string &from, EnumProperty &to);
};

/**
 * @brief Converter to convert an enumeration to integer 32.
 *
 */
template<>
struct Converter<EnumProperty, int32_t>
{
    static void convert(const EnumProperty &from, int32_t &to);
};

/**
 * @brief Converter to assign a new integer value to an enumeration.
 *
 */
template<>
struct Converter<int32_t, EnumProperty>
{
    static void convert(int32_t from, EnumProperty &to);
};

/**
 * @brief Converter to convert an enumeration to integer 64.
 *
 */
template<>
struct Converter<EnumProperty, int64_t>
{
    static void convert(const EnumProperty &from, int64_t &to);
};

/**
 * @brief Converter to assign a new integer value to an enumeration.
 *
 */
template<>
struct Converter<int64_t, EnumProperty>
{
    static void convert(int64_t from, EnumProperty &to);
};
} // namespace brayns