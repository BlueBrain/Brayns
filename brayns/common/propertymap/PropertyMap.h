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

#include <functional>
#include <string>

#include "Any.h"
#include "EnumProperty.h"

namespace brayns
{
/**
 * @brief Shorthand to add label and description to a Property.
 *
 */
struct MetaData
{
    /**
     * @brief The label of the property (longname not key).
     *
     */
    std::string label;

    /**
     * @brief The description of the property.
     *
     */
    std::string description = "no-description";
};

/**
 * @brief Represent a property of an object. Contains mainly a name, a label, a
 * description and a value of any type.
 *
 */
class Property
{
public:
    /**
     * @brief Functor used to notify when the propery value changes.
     *
     */
    using Callback = std::function<void(const Property &)>;

    /**
     * @brief Construct a property from its name, type and optional metadata.
     *
     * @tparam T The type the property is allowed to hold.
     * @param name The key of the property (!= longname from metadata)
     * @param value The value the property is initialized with.
     * @param metaData Optional metadata for high-level description.
     */
    template<typename T>
    Property(std::string name, T value, MetaData metaData = {})
        : _name(std::move(name))
        , _label(std::move(metaData.label))
        , _description(std::move(metaData.description))
        , _value(std::move(value))
    {
    }

    /**
     * @brief Construct an enum property from its name, value and optional
     * metadata.
     *
     * @param name The key of the property.
     * @param value The value of the property.
     * @param metaData Optional metadata.
     */
    Property(std::string name, EnumProperty value, MetaData metaData = {});

    /**
     * @brief Get the key/name of the property.
     *
     * @return const std::string& The name of the property (ex: "minValue")
     */
    const std::string &getName() const;

    /**
     * @brief Get the label set to the property.
     *
     * @return const std::string& Property label.
     */
    const std::string &getLabel() const;

    /**
     * @brief Get the description set to the property.
     *
     * @return const std::string& Property description.
     */
    const std::string &getDescription() const;

    /**
     * @brief Get the type of the property.
     *
     * @return std::type_index The type of the property (cannot be changed).
     */
    std::type_index getType() const;

    /**
     * @brief Get the current value of the property.
     *
     * @return const Any& The value of the property as a generic type.
     */
    const Any &getValue() const;

    /**
     * @brief Update the value of the property and notify if a callback is set.
     *
     * @param value The new value of the property, must be of the exact same
     * type as the current one.
     * @throw std::runtime_error Invalid type.
     */
    void setValue(Any value);

    /**
     * @brief Same as setValue but perform authorized conversion using
     * ConversionRegistry. The type of the property is not changed.
     *
     * @param value The new value of the property, must be of a compatible type
     * with the current one.
     * @throw std::runtime_error Invalid conversion (not in ConversionRegistry).
     */
    void copyValue(const Any &value);

    /**
     * @brief Check if the property is read only. Not checked, info only.
     *
     * @return true The property is read only.
     * @return false The property can be written.
     */
    bool isReadOnly() const;

    /**
     * @brief Set the property as readonly. Not checked, info only.
     *
     * @param readOnly true if readonly.
     */
    void setReadOnly(bool readOnly);

    /**
     * @brief Set a callback to be called when the value is updated.
     *
     * @param callback The callback to set (signature: void(const Property&)).
     */
    void onModified(Callback callback);

    /**
     * @brief Trigger manually the modification callback if any.
     *
     */
    void notifyModification();

    /**
     * @brief Convert the underlying value to the type of the provided one.
     *
     * @param value Output result.
     * @return true Success.
     * @return false Failure.
     */
    bool to(Any &value) const;

    /**
     * @brief Shorthand to check the type of the property.
     *
     * @tparam T The type to check.
     * @return true The property is of type T.
     * @return false The property is not of type T.
     */
    template<typename T>
    bool is() const
    {
        return _value.is<T>();
    }

    /**
     * @brief Cast the value contained by the property.
     *
     * @tparam T The desired type, must be the exact same type of the property.
     * @return const T& Immutable reference on the contained value.
     */
    template<typename T>
    const T &as() const
    {
        return _value.as<T>();
    }

    /**
     * @brief Cast the value contained by the property.
     *
     * @tparam T The desired type, must be the exact same type of the property.
     * @return const T& Mutable reference on the contained value.
     */
    template<typename T>
    T &as()
    {
        return _value.as<T>();
    }

    /**
     * @brief Convert the value contained by the property.
     *
     * @tparam T The result type, the conversion from property type to T must be
     * registered in ConversionRegistry.
     * @return T The contained value converted to the desired type.
     * @throw std::runtime_error The conversion from getType() to T is invalid.
     */
    template<typename T>
    T to() const
    {
        Any result = T();
        if (!to(result))
        {
            throw std::runtime_error("Invalid conversion");
        }
        return result.as<T>();
    }

    /**
     * @brief Call the provided functor with contained value if this one is of
     * type T.
     *
     * @tparam T The type the functor expects.
     * @tparam FunctorType A functor with a signature void(const T&).
     * @param functor The functor instance.
     * @return true The functor has been called.
     * @return false The functor has not been called.
     */
    template<typename T, typename FunctorType>
    bool visit(FunctorType functor) const
    {
        if (!is<T>())
        {
            return false;
        }
        functor(as<T>());
        return true;
    }

    /**
     * @brief Call the provided functor with contained value if this one is of
     * type T.
     *
     * @tparam T The type the functor expects.
     * @tparam FunctorType A functor with a signature void(T&).
     * @param functor The functor instance.
     * @return true The functor has been called.
     * @return false The functor has not been called.
     */
    template<typename T, typename FunctorType>
    bool visit(FunctorType functor)
    {
        if (!is<T>())
        {
            return false;
        }
        functor(as<T>());
        return true;
    }

private:
    std::string _name;
    std::string _label;
    std::string _description;
    Any _value;
    bool _readOnly = false;
    Callback _callback;
};

/**
 * @brief Generic object descriptor (~map<string, Property>) with a name.
 *
 */
class PropertyMap
{
public:
    /**
     * @brief Construct an empty property map with no name.
     *
     */
    PropertyMap() = default;

    /**
     * @brief Construct an empty property map with a name.
     *
     * @param name The name of the property map.
     */
    PropertyMap(std::string name);

    /**
     * @brief Get the name of the property map.
     *
     * @return const std::string& The name of the property map.
     */
    const std::string &getName() const;

    /**
     * @brief Iterator on all immutable properties contained in the map.
     *
     * @return auto Iterator on the properties.
     */
    std::vector<Property>::const_iterator begin() const;

    /**
     * @brief Iterator on all mutable properties contained in the map.
     *
     * @return auto Iterator on the properties.
     */
    std::vector<Property>::iterator begin();

    /**
     * @brief Invalid const iterator.
     *
     * @return auto Invalid iterator.
     */
    std::vector<Property>::const_iterator end() const;

    /**
     * @brief Invalid iterator.
     *
     * @return auto Invalid iterator.
     */
    std::vector<Property>::iterator end();

    /**
     * @brief Get the number of properties inside the map.
     *
     * @return size_t Current number of properties.
     */
    size_t size() const;

    /**
     * @brief Check if the map is empty.
     *
     * @return true The map is empty.
     * @return false The map has at least one property.
     */
    bool empty() const;

    /**
     * @brief Check if an index is valid to access a property.
     *
     * @param index The index to validate.
     * @return true The index is valid and can be used.
     * @return false The index is invalid.
     */
    bool isValidIndex(int index) const;

    /**
     * @brief Get the index of the property with given name.
     *
     * @param name The key of the property to find.
     * @return int The index of the property or -1 if not found.
     */
    int getIndex(const std::string &name) const;

    /**
     * @brief Find a property from its index.
     *
     * @param index The index of the property.
     * @return const Property* A pointer to the property or null if not found.
     */
    const Property *find(int index) const;

    /**
     * @brief Find a mutable property from its index.
     *
     * @param index The index of the property.
     * @return Property* A pointer to the property or null if not found.
     */
    Property *find(int index);

    /**
     * @brief Find a property from its name / key.
     *
     * @param index The name of the property.
     * @return const Property* A pointer to the property or null if not found.
     */
    const Property *find(const std::string &name) const;

    /**
     * @brief Find a mutable property from its name / key.
     *
     * @param index The index of the property.
     * @return Property* A pointer to the property or null if not found.
     */
    Property *find(const std::string &name);

    /**
     * @brief Add a new property to the map. If already present, the existing
     * one will have its value updated. If the types are not compatible, an
     * exception is thrown.
     *
     * @param property The new property to add.
     * @return Property& A reference on the resulting property.
     * @throw std::runtime_error The property already exists with a different
     * type.
     */
    Property &add(const Property &property);

    /**
     * @brief Update the property with the given name with the given value. Does
     * nothing if the property is not found. Throw if incompatible types.
     *
     * @param name The name of the property to update.
     * @param value The new value to set to the property.
     * @throw std::runtime_error The property has an incompatible type with
     * value.
     */
    void update(const std::string &name, const Any &value);

    /**
     * @brief Take all properties from other and add or update current ones. The
     * types must be compatibles.
     *
     * @param other The source properties.
     */
    void merge(const PropertyMap &other);

    /**
     * @brief Take all properties from other and update current ones if exists
     * with the same key. The types must be compatibles.
     *
     * @param other The source properties.
     */
    void update(const PropertyMap &other);

    /**
     * @brief Access property at index (must be valid).
     *
     * @param index A valid index of a property.
     * @return const Property& The desired property.
     */
    const Property &operator[](size_t index) const;

    /**
     * @brief Access the property with name (must be valid).
     *
     * @param name A valid name of a property.
     * @return const Property& The desired property.
     */
    const Property &operator[](const std::string &name) const;

    /**
     * @brief Get the value of the property with name or defaultValue if not
     * found.
     *
     * @tparam The type to convert the property value to.
     * @param name The key of the property to read.
     * @param defaultValue The value if the property is not found.
     * @return T The value of the property or defaultValue if not found.
     * @throw std::runtime_error Invalid conversion (see ConversionRegistry).
     */
    template<typename T>
    T valueOr(const std::string &name, T defaultValue) const
    {
        auto property = find(name);
        if (!property)
        {
            return defaultValue;
        }
        return property->to<T>();
    }

private:
    std::string _name;
    std::vector<Property> _properties;
};
} // namespace brayns