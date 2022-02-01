/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

#include "ConversionRegistry.h"

#include <functional>
#include <typeindex>
#include <unordered_map>

#include "Converter.h"
#include "EnumProperty.h"

namespace
{
struct Conversion
{
    std::type_index from = typeid(void);

    std::type_index to = typeid(void);

    bool operator==(const Conversion &other) const
    {
        return from == other.from && to == other.to;
    }
};
} // namespace

namespace std
{
/**
 * @brief Custom hasher for Conversion (~hash_combine).
 *
 * @note The hash function is a bit overcomplicated to avoid hash(From, To) ==
 * hash(To, From).
 *
 * @tparam  Conversion to hash.
 */
template<>
struct hash<Conversion>
{
    size_t operator()(const Conversion &conversion) const
    {
        constexpr size_t randomValue = 0x9e3779b9;
        auto from = conversion.from.hash_code();
        auto to = conversion.to.hash_code();
        return from ^ (to + randomValue + (from << 6) + (from >> 2));
    }
};
} // namespace std

namespace
{
using Any = brayns::Any;

class AnyConverter
{
public:
    template<typename From, typename To, typename FunctorType>
    static AnyConverter create(FunctorType functor)
    {
        AnyConverter converter;
        converter._conversion = {typeid(From), typeid(To)};
        converter._functor = [functor = std::move(functor)](const Any &from, Any &to)
        { functor(from.as<From>(), to.as<To>()); };
        return converter;
    }

    template<typename From, typename To>
    static AnyConverter create()
    {
        return create<From, To>([](const auto &from, auto &to) { brayns::Converter<From, To>::convert(from, to); });
    }

    const Conversion &getConversion() const
    {
        return _conversion;
    }

    void convert(const Any &from, Any &to) const
    {
        assert(from.getType() == _conversion.from);
        assert(to.getType() == _conversion.to);
        _functor(from, to);
    }

private:
    Conversion _conversion;
    std::function<void(const Any &, Any &)> _functor;
};

class AnyConverterMap
{
public:
    bool convert(const Any &from, Any &to)
    {
        Conversion conversion = {from.getType(), to.getType()};
        if (conversion.from == conversion.to)
        {
            to = from;
            return true;
        }
        auto i = _converters.find(conversion);
        if (i == _converters.end())
        {
            return false;
        }
        i->second.convert(from, to);
        return true;
    }

    void add(AnyConverter converter)
    {
        _converters[converter.getConversion()] = std::move(converter);
    }

    template<typename From, typename To>
    void add()
    {
        add(AnyConverter::create<From, To>());
    }

    template<typename From, typename To>
    void add2()
    {
        add<From, To>();
        add<To, From>();
    }

    template<typename From, typename To>
    void addex()
    {
        add<From, To>();
        add<std::vector<From>, std::vector<To>>();
    }

    template<typename From, typename To>
    void addex2()
    {
        addex<From, To>();
        addex<To, From>();
    }

    template<typename From, typename To>
    void addexn()
    {
        addex<From, To>();
        add<glm::vec<2, From>, glm::vec<2, To>>();
        add<glm::vec<3, From>, glm::vec<3, To>>();
        add<glm::vec<4, From>, glm::vec<4, To>>();
    }

    template<typename From, typename To>
    void addexn2()
    {
        addexn<From, To>();
        addexn<To, From>();
    }

private:
    std::unordered_map<Conversion, AnyConverter> _converters;
};

AnyConverterMap _registerConverters()
{
    AnyConverterMap converters;
    converters.addexn2<int32_t, int64_t>();
    converters.addexn2<int32_t, double>();
    converters.addexn2<int64_t, double>();
    converters.addex2<int32_t, brayns::EnumProperty>();
    converters.addex2<int64_t, brayns::EnumProperty>();
    converters.addex2<std::string, brayns::EnumProperty>();
    return converters;
}

AnyConverterMap _converters = _registerConverters();
} // namespace

namespace brayns
{
bool ConversionRegistry::convert(const Any &from, Any &to)
{
    return _converters.convert(from, to);
}
} // namespace brayns
