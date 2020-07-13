/* Copyright (c) 2015-2019, EPFL/Blue Brain Project
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

#include "PropertyMap.h"

#include "log.h"
#include "utils/stringUtils.h"

#include <array>
#include <iomanip>

#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>
#include <brayns/parameters/AbstractParameters.h>

namespace brayns
{
namespace
{
template <typename TDest, typename TSrc, size_t S>
std::array<TDest, S> _convertArray(const std::array<TSrc, S> src)
{
    std::array<TDest, S> dest;
    for (size_t i = 0; i < S; i++)
        dest[i] = static_cast<TDest>(src[i]);
    return dest;
}

template <typename T, std::size_t N>
auto _toStdArray(const std::vector<T>& input)
{
    if (input.size() != N)
        throw po::error("Not the correct number of args for property");
    std::array<T, N> array;
    std::copy(input.begin(), input.end(), array.begin());
    return array;
}

po::options_description _toCommandlineDescription(
    const PropertyMap& propertyMap)
{
    po::options_description desc(propertyMap.getName());
    for (const auto& property : propertyMap.getProperties())
    {
        po::value_semantic* valueSemantic{nullptr};
        switch (property->type)
        {
        case Property::Type::Int:
        {
            if (property->enums.empty())
                valueSemantic = po::value<int32_t>()->default_value(
                    property->get<int32_t>());
            else
                valueSemantic = po::value<std::string>()->default_value(
                    property->enums[property->get<int32_t>()]);
            break;
        }
        case Property::Type::UInt:
        {
            if (property->enums.empty())
                valueSemantic = po::value<uint32_t>()->default_value(
                    property->get<uint32_t>());
            else
                valueSemantic = po::value<std::string>()->default_value(
                    property->enums[property->get<uint32_t>()]);
            break;
        }
        case Property::Type::UInt64:
        {
            if (property->enums.empty())
                valueSemantic = po::value<uint64_t>()->default_value(
                    property->get<uint64_t>());
            else
                valueSemantic = po::value<std::string>()->default_value(
                    property->enums[property->get<uint64_t>()]);
            break;
        }
        case Property::Type::Double:
            valueSemantic =
                po::value<double>()->default_value(property->get<double>());
            break;
        case Property::Type::String:
            valueSemantic = po::value<std::string>()->default_value(
                property->get<std::string>());
            break;
        case Property::Type::Bool:
            // default true bools cannot be switched off with bool_switch
            if (property->get<bool>())
                valueSemantic = po::value<bool>()->default_value(true);
            else
                valueSemantic = po::bool_switch();
            break;
        case Property::Type::Vec2i:
            valueSemantic = po::fixed_tokens_value<ints>(2, 2);
            break;
        case Property::Type::Vec3i:
            valueSemantic = po::fixed_tokens_value<ints>(3, 3);
            break;
        case Property::Type::Vec2d:
            valueSemantic = po::fixed_tokens_value<std::vector<double>>(2, 2);
            break;
        case Property::Type::Vec3d:
            valueSemantic = po::fixed_tokens_value<std::vector<double>>(3, 3);
            break;
        case Property::Type::Vec4d:
            valueSemantic = po::fixed_tokens_value<std::vector<double>>(4, 4);
            break;
        case Property::Type::DoubleVector:
            {
                auto max = static_cast<uint32_t>(std::numeric_limits<uint32_t>::max() - 1);
                valueSemantic = po::fixed_tokens_value<std::vector<double>>(1, max);
            }
            break;
        case Property::Type::FloatVector:
            {
                auto max = static_cast<uint32_t>(std::numeric_limits<uint32_t>::max() - 1);
                valueSemantic = po::fixed_tokens_value<std::vector<float>>(1, max);
            }
            break;
        case Property::Type::IntVector:
            {
                auto max = static_cast<uint32_t>(std::numeric_limits<uint32_t>::max() - 1);
                valueSemantic = po::fixed_tokens_value<std::vector<int32_t>>(1, max);
            }
            break;
        case Property::Type::UIntVector:
            {
                auto max = static_cast<uint32_t>(std::numeric_limits<uint32_t>::max() - 1);
                valueSemantic = po::fixed_tokens_value<std::vector<uint32_t>>(1, max);
            }
            break;
        case Property::Type::UInt64Vector:
            {
                auto max = static_cast<uint32_t>(std::numeric_limits<uint32_t>::max() - 1);
                valueSemantic = po::fixed_tokens_value<std::vector<uint64_t>>(1, max);
            }
            break;
        case Property::Type::StringVector:
            {
                auto max = static_cast<unsigned>(std::numeric_limits<uint32_t>::max() - 1);
                valueSemantic = po::fixed_tokens_value<std::vector<std::string>>(1, max);
            }
            break;
        case Property::Type::BoolVector:
            {
                auto max = static_cast<uint32_t>(std::numeric_limits<uint32_t>::max() - 1);
                valueSemantic = po::fixed_tokens_value<std::vector<bool>>(1, max);
            }
            break;
        }

        assert(valueSemantic);
        const auto dashCaseName =
            string_utils::camelCaseToSeparated(property->name, '-');
        desc.add(boost::make_shared<po::option_description>(
            dashCaseName.c_str(), valueSemantic,
            property->metaData.description.c_str()));
    }
    return desc;
}

auto _validateEnumValue(const strings& enums, const std::string& value,
                        const std::string& optionName)
{
    auto i = std::find(enums.begin(), enums.end(), value);
    if (i == enums.end())
        throw po::error(optionName + " must be one of the following: " +
                        string_utils::join(enums, ", "));
    return i;
}

void _commandlineToPropertyMap(const po::variables_map& vm,
                               PropertyMap& propertyMap)
{
    for (const auto& property : propertyMap.getProperties())
    {
        const auto dashCaseName =
            string_utils::camelCaseToSeparated(property->name, '-');
        if (!vm.count(dashCaseName))
            continue;
        switch (property->type)
        {
        case Property::Type::Int:
            if (property->enums.empty())
                property->set(vm[dashCaseName].as<int32_t>());
            else
            {
                const auto value = vm[dashCaseName].as<std::string>();
                auto i =
                    _validateEnumValue(property->enums, value, dashCaseName);
                property->set((int32_t)(i - property->enums.begin()));
            }
            break;
        case Property::Type::UInt:
            if (property->enums.empty())
                property->set(vm[dashCaseName].as<uint32_t>());
            else
            {
                const auto value = vm[dashCaseName].as<std::string>();
                auto i =
                    _validateEnumValue(property->enums, value, dashCaseName);
                property->set((uint32_t)(i - property->enums.begin()));
            }
            break;
        case Property::Type::UInt64:
            if (property->enums.empty())
                property->set(vm[dashCaseName].as<uint64_t>());
            else
            {
                const auto value = vm[dashCaseName].as<std::string>();
                auto i =
                    _validateEnumValue(property->enums, value, dashCaseName);
                property->set((uint64_t)(i - property->enums.begin()));
            }
            break;
        case Property::Type::Double:
            property->set(vm[dashCaseName].as<double>());
            break;
        case Property::Type::String:
            if (property->enums.empty())
                property->set(vm[dashCaseName].as<std::string>());
            else
            {
                const auto value = vm[dashCaseName].as<std::string>();
                auto i =
                    _validateEnumValue(property->enums, value, dashCaseName);
                property->set(*i);
            }
            break;
        case Property::Type::Bool:
            property->set(vm[dashCaseName].as<bool>());
            break;
        case Property::Type::Vec2i:
            property->set(_toStdArray<int32_t, 2>(
                vm[dashCaseName].as<std::vector<int32_t>>()));
            break;
        case Property::Type::Vec2d:
            property->set(_toStdArray<double, 2>(
                vm[dashCaseName].as<std::vector<double>>()));
            break;
        case Property::Type::Vec3i:
            property->set(_toStdArray<int32_t, 3>(
                vm[dashCaseName].as<std::vector<int32_t>>()));
            break;
        case Property::Type::Vec3d:
            property->set(_toStdArray<double, 3>(
                vm[dashCaseName].as<std::vector<double>>()));
            break;
        case Property::Type::Vec4d:
            property->set(_toStdArray<double, 4>(
                vm[dashCaseName].as<std::vector<double>>()));
            break;
        case Property::Type::DoubleVector:
            property->set(vm[dashCaseName].as<std::vector<double>>());
            break;
        case Property::Type::FloatVector:
            property->set(vm[dashCaseName].as<std::vector<float>>());
            break;
        case Property::Type::IntVector:
            property->set(vm[dashCaseName].as<std::vector<int32_t>>());
            break;
        case Property::Type::UIntVector:
            property->set(vm[dashCaseName].as<std::vector<uint32_t>>());
            break;
        case Property::Type::UInt64Vector:
            property->set(vm[dashCaseName].as<std::vector<uint64_t>>());
            break;
        case Property::Type::StringVector:
            property->set(vm[dashCaseName].as<std::vector<std::string>>());
            break;
        case Property::Type::BoolVector:
            property->set(vm[dashCaseName].as<std::vector<bool>>());
            break;
        }
    }
}
}

void Property::_copy(const Property& from)
{
    const auto setValue = [](Property& dest, const Property& src) {
        switch (dest.type)
        {
        case Property::Type::Int:
            dest.set<int32_t>(static_cast<int32_t>(src.get<double>()));
            break;
        case Property::Type::UInt:
            dest.set<uint32_t>(static_cast<uint32_t>(src.get<double>()));
            break;
        case Property::Type::UInt64:
            dest.set<uint64_t>(static_cast<uint64_t>(src.get<double>()));
            break;
        case Property::Type::Double:
            dest.set<double>(static_cast<double>(src.get<int32_t>()));
            break;
        case Property::Type::Vec2i:
            dest.set<std::array<int32_t, 2>>(_convertArray<int32_t, double, 2>(
                src.get<std::array<double, 2>>()));
            break;
        case Property::Type::Vec2d:
            dest.set<std::array<double, 2>>(_convertArray<double, int32_t, 2>(
                src.get<std::array<int32_t, 2>>()));
            break;
        case Property::Type::Vec3i:
            dest.set<std::array<int32_t, 3>>(_convertArray<int32_t, double, 3>(
                src.get<std::array<double, 3>>()));
            break;
        case Property::Type::Vec3d:
            dest.set<std::array<double, 3>>(_convertArray<double, int32_t, 3>(
                src.get<std::array<int32_t, 3>>()));
            break;
        default:
            break;
        };
    };

    const auto compatibleTypes = [](Property::Type t0, Property::Type t1) {
        return (t0 == Property::Type::Int && t1 == Property::Type::Double) ||
               (t0 == Property::Type::Double && t1 == Property::Type::Int) ||
               (t0 == Property::Type::Vec2i && t1 == Property::Type::Vec2d) ||
               (t0 == Property::Type::Vec2d && t1 == Property::Type::Vec2i) ||
               (t0 == Property::Type::Vec3i && t1 == Property::Type::Vec3d) ||
               (t0 == Property::Type::Vec3d && t1 == Property::Type::Vec3i);
    };

    const auto compatibleEnums = [](const Property& dest, const Property& src) {
        // If we have a string to int or an int to string we can try to
        // match the enum value
        const bool compatible = (dest.type == Property::Type::Int &&
                                 src.type == Property::Type::String) ||
                                (dest.type == Property::Type::String &&
                                 src.type == Property::Type::Int);
        if (dest.enums.empty() || !compatible)
            return false;

        // If our source is a string we check if the string exist in destination
        if (src.type == Property::Type::String)
            return std::find(dest.enums.begin(), dest.enums.end(),
                             src.get<std::string>()) != dest.enums.end();

        // We know source is an int so check that the range is inside enum range
        const int32_t v = src.get<int32_t>();
        return v >= 0 && v < static_cast<int32_t>(dest.enums.size());
    };

    const auto setEnum = [](Property& dest, const Property& src) {
        if (dest.type == Property::Type::Int)
        {
            const auto index = std::find(dest.enums.begin(), dest.enums.end(),
                                         src.get<std::string>()) -
                               dest.enums.begin();
            dest.set<int32_t>(index);
        }
        else
        {
            const auto idx = src.get<int32_t>();
            dest.set(dest.enums[idx]);
        }
    };

    if (from.type == type)
        _setData(from._data);
    else if (compatibleEnums(*this, from))
        setEnum(*this, from);
    else if (compatibleTypes(type, from.type))
        setValue(*this, from);
    else
    {
        throw std::runtime_error("Incompatible types for property '" + name +
                                 "'");
    }
}

void PropertyMap::merge(const PropertyMap& input)
{
    for (const auto& otherProperty : input.getProperties())
    {
        const auto& name = otherProperty->name;

        if (auto myProperty = find(name))
            myProperty->_copy(*otherProperty);
        else
            setProperty(*otherProperty.get());
    }
}

void PropertyMap::update(const PropertyMap& input)
{
    for (const auto& otherProperty : input.getProperties())
    {
        if (auto myProperty = find(otherProperty->name))
            myProperty->_copy(*otherProperty);
    }
}

bool PropertyMap::parse(const int argc, const char** argv)
{
    try
    {
        auto desc = _toCommandlineDescription(*this);
        desc.add_options()("help", "Print this help");
        const auto parsedOptions =
            po::command_line_parser(argc, argv).options(desc).run();

        po::variables_map vm;
        po::store(parsedOptions, vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return false;
        }

        _commandlineToPropertyMap(vm, *this);
        return true;
    }
    catch (const po::error& e)
    {
        BRAYNS_ERROR << "Failed to parse commandline for "
                     << std::quoted(getName()) << ": " << e.what() << std::endl;
        return false;
    }
}
}
