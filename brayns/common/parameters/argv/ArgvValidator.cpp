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

#include "ArgvValidator.h"

#include <sstream>

namespace
{
class ArgvErrors
{
public:
    std::vector<std::string> extract()
    {
        return std::move(_errors);
    }

    void unknownKey(const std::string &key)
    {
        _errors.push_back("Unknown keyword argument: '" + key + "'");
    }

    void notEnoughItems(const brayns::ArgvProperty &property, size_t count)
    {
        std::ostringstream stream;
        stream << "Not enough items for '" << property.name << "': " << count << " < " << *property.minItems;
        _errors.push_back(stream.str());
    }

    void tooManyItems(const brayns::ArgvProperty &property, size_t count)
    {
        std::ostringstream stream;
        stream << "Too many items for '" << property.name << "': " << count << " > " << *property.maxItems;
        _errors.push_back(stream.str());
    }

    void invalidEnum(const brayns::ArgvProperty &property, const std::string &value)
    {
        std::ostringstream stream;
        stream << "Invalid enum for '" << property.name << "': '" << value << "' not in [";
        bool first = true;
        for (const auto &name : property.enums)
        {
            if (!first)
            {
                stream << ", ";
            }
            first = false;
            stream << "'" << name << "'";
        }
        stream << "]";
        _errors.push_back(stream.str());
    }

    void invalidType(const brayns::ArgvProperty &property, const std::string &value)
    {
        std::ostringstream stream;
        stream << "Invalid type for '" << property.name;
        stream << "': cannot convert '" << value;
        stream << "' to " << brayns::EnumInfo::getName(property.type);
        _errors.push_back(stream.str());
    }

    void belowMinimum(const brayns::ArgvProperty &property, double value)
    {
        std::ostringstream stream;
        stream << "Value below minimum for '" << property.name;
        stream << "': " << value << " < " << *property.minimum;
        _errors.push_back(stream.str());
    }

    void aboveMaximum(const brayns::ArgvProperty &property, double value)
    {
        std::ostringstream stream;
        stream << "Value above maximum for '" << property.name;
        stream << "': " << value << " > " << *property.maximum;
        _errors.push_back(stream.str());
    }

private:
    std::vector<std::string> _errors;
};

class ArgvKeyValidator
{
public:
    static std::vector<std::string> getUnknownKeys(
        const brayns::Argv &argv,
        const std::vector<brayns::ArgvProperty> &properties)
    {
        std::vector<std::string> keys;
        for (const auto &[key, values] : argv)
        {
            if (!_find(key, properties))
            {
                keys.push_back(key);
            }
        }
        return keys;
    }

private:
    static bool _find(const std::string &key, const std::vector<brayns::ArgvProperty> &properties)
    {
        auto first = properties.begin();
        auto last = properties.end();
        auto predictor = [&](auto &property) { return property.name == key; };
        return std::find_if(first, last, predictor) != last;
    }
};

class ArgvTypeChecker
{
public:
    static bool checkType(const std::string &value, brayns::ArgvType type)
    {
        switch (type)
        {
        case brayns::ArgvType::Boolean:
            return _checkBoolean(value);
        case brayns::ArgvType::Integer:
            return _checkInteger(value);
        case brayns::ArgvType::Number:
            return _checkNumber(value);
        default:
            return true;
        }
    }

private:
    static bool _checkBoolean(const std::string &value)
    {
        return value == "true" || value == "false" || value == "1" || value == "0";
    }

    static bool _checkInteger(const std::string &value)
    {
        return !value.empty() && std::all_of(value.begin(), value.end(), [](auto c) { return std::isdigit(c); });
    }

    static bool _checkNumber(const std::string &value)
    {
        std::istringstream stream(value);
        double number;
        stream >> number;
        return stream.good();
    }
};

class ArgvValidatorHelper
{
public:
    std::vector<std::string> validate(const brayns::Argv &argv, const std::vector<brayns::ArgvProperty> &properties)
    {
        _validateKeys(argv, properties);
        for (const auto &property : properties)
        {
            auto &name = property.name;
            auto i = argv.find(name);
            if (i == argv.end())
            {
                continue;
            }
            auto &values = i->second;
            _validate(values, property);
        }
        return _errors.extract();
    }

private:
    ArgvErrors _errors;

    void _validateKeys(const brayns::Argv &argv, const std::vector<brayns::ArgvProperty> &properties)
    {
        auto unknownKeys = ArgvKeyValidator::getUnknownKeys(argv, properties);
        for (const auto &key : unknownKeys)
        {
            _errors.unknownKey(key);
        }
    }

    void _validate(const std::vector<std::string> &values, const brayns::ArgvProperty &property)
    {
        _validateItemCount(values, property);
        if (!property.enums.empty())
        {
            _validateEnums(values, property);
            return;
        }
        _validateType(values, property);
        _validateBounds(values, property);
    }

    void _validateItemCount(const std::vector<std::string> &values, const brayns::ArgvProperty &property)
    {
        auto count = values.size();
        if (property.minItems && count < *property.minItems)
        {
            _errors.notEnoughItems(property, count);
        }
        if (property.maxItems && count > *property.maxItems)
        {
            _errors.tooManyItems(property, count);
        }
    }

    void _validateEnums(const std::vector<std::string> &values, const brayns::ArgvProperty &property)
    {
        auto &enums = property.enums;
        for (const auto &value : values)
        {
            auto i = std::find(enums.begin(), enums.end(), value);
            if (i == enums.end())
            {
                _errors.invalidEnum(property, value);
            }
        }
    }

    void _validateType(const std::vector<std::string> &values, const brayns::ArgvProperty &property)
    {
        auto type = property.type;
        for (const auto &value : values)
        {
            if (!ArgvTypeChecker::checkType(value, type))
            {
                _errors.invalidType(property, value);
            }
        }
    }

    void _validateBounds(const std::vector<std::string> &values, const brayns::ArgvProperty &property)
    {
        for (const auto &value : values)
        {
            double number = 0.0;
            std::istringstream(value) >> number;
            if (property.minimum && number < *property.minimum)
            {
                _errors.belowMinimum(property, number);
            }
            if (property.maximum && number > *property.maximum)
            {
                _errors.aboveMaximum(property, number);
            }
        }
    }
};
} // namespace

namespace brayns
{
std::vector<std::string> ArgvValidator::validate(const Argv &argv, const std::vector<ArgvProperty> &properties)
{
    ArgvValidatorHelper helper;
    return helper.validate(argv, properties);
}
} // namespace brayns
