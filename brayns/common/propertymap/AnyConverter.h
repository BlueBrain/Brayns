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

#include <functional>
#include <typeindex>
#include <unordered_map>

#include "Any.h"
#include "Converter.h"

namespace brayns
{
/**
 * @brief Store the source and destination type of a conversion to represent it
 * at runtime.
 *
 */
struct Conversion
{
    /**
     * @brief Source type of the conversion.
     *
     */
    std::type_index from = typeid(void);

    /**
     * @brief Destination type of the conversion.
     *
     */
    std::type_index to = typeid(void);

    /**
     * @brief Compare two conversions and check that from and to are the same
     * (ie the two conversions are equivalent).
     *
     * @param other Right hand side operand.
     * @return true If from == other.from and to == other.to.
     * @return false Otherwise.
     */
    bool operator==(const Conversion& other) const
    {
        return from == other.from && to == other.to;
    }
};
} // namespace brayns

namespace std
{
/**
 * @brief Custom hasher for Conversion (same as boost::hash_combine).
 *
 * @note The hash function is a bit overcomplicated to avoid hash(From, To) ==
 * hash(To, From).
 *
 * @tparam  Conversion to hash.
 */
template <>
struct hash<brayns::Conversion>
{
    size_t operator()(const brayns::Conversion& conversion) const
    {
        constexpr size_t randomValue = 0x9e3779b9;
        auto from = conversion.from.hash_code();
        auto to = conversion.to.hash_code();
        return from ^ (to + randomValue + (from << 6) + (from >> 2));
    }
};
} // namespace std

namespace brayns
{
/**
 * @brief Class used to perform a conversion between two different types at
 * runtime. Runtime types are represented using the Any class. A dynamic
 * conversion has hence the generic signature void(const Any&, Any&). Can be
 * constructed using static converter.
 *
 */
class AnyConverter
{
public:
    /**
     * @brief Creates a converter of From type to To type using the provided
     * functor.
     *
     * @tparam From The conversion source type.
     * @tparam To The conversion destination type.
     * @tparam FunctorType The type of the functor used for auto-deduction.
     * @param functor  The functor that performs the conversion, must have the
     * signature compatible with void(const From&, To&).
     * @return AnyConverter The converter that will perform the conversion from
     * From to To in runtime.
     */
    template <typename From, typename To, typename FunctorType>
    static AnyConverter create(FunctorType functor)
    {
        AnyConverter converter;
        converter._conversion = {typeid(From), typeid(To)};
        converter._functor = [functor = std::move(functor)](const Any& from,
                                                            Any& to) {
            functor(from.as<From>(), to.as<To>());
        };
        return converter;
    }

    /**
     * @brief Create an AnyConverter using the static Converter<From, To>.
     *
     * @tparam From The source type.
     * @tparam To The destination type.
     * @return AnyConverter The converter that will perform the conversion from
     * From to To in runtime.
     */
    template <typename From, typename To>
    static AnyConverter create()
    {
        return create<From, To>([](const auto& from, auto& to) {
            Converter<From, To>::convert(from, to);
        });
    }

    /**
     * @brief Get the Conversion describing the conversion performed by the
     * converter.
     *
     * @return const Conversion& The conversion runtime conversion.
     */
    const Conversion& getConversion() const { return _conversion; }

    /**
     * @brief Perform the conversion between two Any instances having the same
     * types as the one the converter expects. If the types of the Any are not
     * the ones of the converter, it is undefined behavior.
     *
     * @param from
     * @param to
     */
    void convert(const Any& from, Any& to) const
    {
        assert(from.getType() == _conversion.from);
        assert(to.getType() == _conversion.to);
        _functor(from, to);
    }

private:
    Conversion _conversion;
    std::function<void(const Any&, Any&)> _functor;
};

/**
 * @brief Represents a mapping from Conversion to AnyConverter to perform
 * conversion at runtime between compatible (registered) types.
 *
 */
class AnyConverterMap
{
public:
    /**
     * @brief Fetch the converter based on from and to stored types, perform the
     * conversion and return wether the conversion was done or not.
     *
     * @param from The source value.
     * @param to The destination value.
     * @return true The converter was found so the conversion is performed.
     * @return false The converter was not found so nothing happened.
     */
    bool convert(const Any& from, Any& to)
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

    /**
     * @brief Add or replace a converter for the conversion it performs.
     *
     * @param converter The converter to register.
     */
    void add(AnyConverter converter)
    {
        _converters[converter.getConversion()] = std::move(converter);
    }

    /**
     * @brief Add or replace a converter using From and To types.
     *
     * @tparam From The source type.
     * @tparam To The destination type.
     */
    template <typename From, typename To>
    void add()
    {
        add(AnyConverter::create<From, To>());
    }

    /**
     * @brief Add or replace a converter using From and To types. Add also the
     * reverse conversion (To -> From).
     *
     * @tparam From The source type.
     * @tparam To The destination type.
     */
    template <typename From, typename To>
    void add2()
    {
        add<From, To>();
        add<To, From>();
    }

    /**
     * @brief Add or replace a converter using from and To types. Add also the
     * vector conversion (vector<From> -> vector<To>).
     *
     * @tparam From The source type.
     * @tparam To The destination type.
     */
    template <typename From, typename To>
    void addex()
    {
        add<From, To>();
        add<std::vector<From>, std::vector<To>>();
    }

    /**
     * @brief Combination of addex and add2 to add vector conversions
     * in both directions (From -> To and To -> From).
     *
     * @tparam From The source type.
     * @tparam To The destination type.
     */
    template <typename From, typename To>
    void addex2()
    {
        addex<From, To>();
        addex<To, From>();
    }

    /**
     * @brief Add or replace a converter using from and To types. Add also the
     * vector conversion (vector<From> -> vector<To>) and the glm::vecN
     * conversions with N = 2, 3, 4.
     *
     * @tparam From The source type.
     * @tparam To The destination type.
     */
    template <typename From, typename To>
    void addexn()
    {
        addex<From, To>();
        add<glm::vec<2, From>, glm::vec<2, To>>();
        add<glm::vec<3, From>, glm::vec<3, To>>();
        add<glm::vec<4, From>, glm::vec<4, To>>();
    }

    /**
     * @brief Combination of addexn and add2 to add vector and vecN conversions
     * in both directions (From -> To and To -> From).
     *
     * @tparam From The source type.
     * @tparam To The destination type.
     */
    template <typename From, typename To>
    void addexn2()
    {
        addexn<From, To>();
        addexn<To, From>();
    }

private:
    std::unordered_map<Conversion, AnyConverter> _converters;
};
} // namespace brayns