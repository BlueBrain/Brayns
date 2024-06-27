/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include <brayns/core/utils/StringParser.h>

namespace brayns
{
using ArgvMap = std::unordered_map<std::string, std::string>;

ArgvMap parseArgv(int argc, const char **argv);

template<typename SettingsType>
struct ArgvOption
{
    std::string key;
    std::string type;
    std::string description;
    std::string defaultValue;
    std::function<void(std::string_view, SettingsType &)> parse;
};

template<typename SettingsType>
using ArgvOptions = std::map<std::string, ArgvOption<SettingsType>>;

template<typename SettingsType>
class ArgvInfo
{
public:
    explicit ArgvInfo(std::string description, ArgvOptions<SettingsType> options):
        _description(std::move(description)),
        _options(std::move(options))
    {
    }

    std::string getHelp() const
    {
        auto stream = std::ostringstream();

        stream << _description << "\n\n";

        stream << "Options:\n";

        for (const auto &[key, option] : _options)
        {
            stream << "    --" << option.key;
            stream << " " << option.type;
            stream << ": " << option.description;
            stream << " (default " << option.defaultValue << ")\n";
        }

        return stream.str();
    }

    SettingsType parse(const ArgvMap &argv) const
    {
        auto result = SettingsType();

        for (const auto &[key, value] : argv)
        {
            if (!_options.contains(key))
            {
                throw std::invalid_argument(fmt::format("Unknown option: '{}'", key));
            }
        }

        for (const auto &[key, option] : _options)
        {
            auto i = argv.find(key);

            if (i == argv.end())
            {
                option.parse(option.defaultValue, result);
                continue;
            }

            option.parse(i->second, result);
        }

        return result;
    }

private:
    std::string _description;
    ArgvOptions<SettingsType> _options;
};

template<typename T>
struct ArgvReflector;

template<typename T>
concept ReflectedArgvOption = std::same_as<std::string, decltype(ArgvReflector<T>::getType())>
    && std::same_as<std::string, decltype(ArgvReflector<T>::display(T()))>
    && std::same_as<T, decltype(ArgvReflector<T>::parse(std::string_view()))>;

template<>
struct ArgvReflector<bool>
{
    static std::string getType()
    {
        return "boolean";
    }

    static std::string display(bool value)
    {
        return value ? "true" : "false";
    }

    static bool parse(std::string_view data)
    {
        if (data.empty())
        {
            return true;
        }

        return parseStringAs<bool>(data);
    }
};

template<typename T>
    requires std::is_arithmetic_v<T>
struct ArgvReflector<T>
{
    static std::string getType()
    {
        return std::is_integral_v<T> ? "integer" : "number";
    }

    static std::string display(const T &value)
    {
        return fmt::format("{}", value);
    }

    static T parse(std::string_view data)
    {
        return parseStringAs<T>(data);
    }
};

template<>
struct ArgvReflector<std::string>
{
    static std::string getType()
    {
        return "string";
    }

    static std::string display(const std::string &value)
    {
        return value;
    }

    static std::string parse(std::string_view data)
    {
        return std::string(data);
    }
};

template<typename SettingsType>
class ArgvOptionBuilder
{
public:
    explicit ArgvOptionBuilder(ArgvOption<SettingsType> &option):
        _option(&option)
    {
    }

    ArgvOptionBuilder description(std::string value)
    {
        _option->description = std::move(value);
        return *this;
    }

    template<ReflectedArgvOption T>
    ArgvOptionBuilder defaultValue(const T &value)
    {
        _option->defaultValue = ArgvReflector<T>::display(value);
        return *this;
    }

    ArgvOptionBuilder defaultValue(const char *value)
    {
        return defaultValue(std::string(value));
    }

private:
    ArgvOption<SettingsType> *_option;
};

template<typename GetterType, typename SettingsType>
using GetOptionType = std::decay_t<std::remove_pointer_t<std::invoke_result_t<GetterType, SettingsType &>>>;

template<typename GetterType, typename SettingsType>
concept ArgvOptionGetter =
    std::invocable<GetterType, SettingsType &> && std::is_pointer_v<std::invoke_result_t<GetterType, SettingsType &>>
    && ReflectedArgvOption<GetOptionType<GetterType, SettingsType>>;

template<typename SettingsType>
class ArgvBuilder
{
public:
    void description(std::string value)
    {
        _description = std::move(value);
    }

    template<ArgvOptionGetter<SettingsType> T>
    ArgvOptionBuilder<SettingsType> option(std::string key, T getOptionPtr)
    {
        using OptionType = GetOptionType<T, SettingsType>;
        using Reflector = ArgvReflector<OptionType>;

        if (_options.contains(key))
        {
            throw std::invalid_argument(fmt::format("Duplicated option: '{}'", key));
        }

        auto &option = _options[key];

        option.key = std::move(key);
        option.type = Reflector::getType();
        option.parse = [=](auto data, auto &settings) { *getOptionPtr(settings) = Reflector::parse(data); };

        return ArgvOptionBuilder<SettingsType>(option);
    }

    ArgvInfo<SettingsType> build()
    {
        return ArgvInfo<SettingsType>(std::exchange(_description, {}), std::exchange(_options, {}));
    }

private:
    std::string _description;
    ArgvOptions<SettingsType> _options;
};

template<typename T>
struct ArgvSettingsReflector;

template<typename T>
concept ReflectedArgvSettings = std::same_as<ArgvInfo<T>, decltype(ArgvSettingsReflector<T>::reflect())>;

template<ReflectedArgvSettings SettingsType>
const ArgvInfo<SettingsType> &reflectArgvSettings()
{
    static const ArgvInfo<SettingsType> settings = ArgvSettingsReflector<SettingsType>::reflect();
    return settings;
}

template<ReflectedArgvSettings SettingsType>
std::string getArgvHelp()
{
    return reflectArgvSettings<SettingsType>().getHelp();
}

template<ReflectedArgvSettings SettingsType>
SettingsType parseArgvAs(const ArgvMap &argv)
{
    return reflectArgvSettings<SettingsType>().parse(argv);
}

template<ReflectedArgvSettings SettingsType>
SettingsType parseArgvAs(int argc, const char **argv)
{
    auto map = parseArgv(argc, argv);
    return parseArgvAs<SettingsType>(map);
}
}
