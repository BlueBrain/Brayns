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

#include <tuple>

namespace brayns
{
template<typename T>
struct FunctorReflector : FunctorReflector<decltype(&T::operator())>
{
};

template<typename ReturnType, typename... Args>
struct FunctorReflector<ReturnType (*)(Args...)> : FunctorReflector<ReturnType(Args...)>
{
};

template<typename ObjectType, typename ReturnType, typename... Args>
struct FunctorReflector<ReturnType (ObjectType::*)(Args...)> : FunctorReflector<ReturnType(Args...)>
{
};

template<typename ObjectType, typename ReturnType, typename... Args>
struct FunctorReflector<ReturnType (ObjectType::*)(Args...) const> : FunctorReflector<ReturnType(Args...)>
{
};

template<typename ReturnType, typename... Args>
struct FunctorReflector<ReturnType(Args...)>
{
    using Return = ReturnType;

    template<std::size_t Index>
    using Arg = std::tuple_element_t<Index, std::tuple<Args...>>;

    static inline constexpr auto argCount = sizeof...(Args);
};

template<typename T>
using GetReturnType = typename FunctorReflector<T>::Return;

template<typename T, std::size_t Index>
using GetArgType = typename FunctorReflector<T>::template Arg<Index>;

template<typename T>
constexpr auto getArgCount = FunctorReflector<T>::argCount;
}
