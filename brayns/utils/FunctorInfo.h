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

#include <tuple>

namespace brayns
{
template <typename R, typename O, typename... A>
struct FunctorTrait
{
    using ReturnType = R;

    using ObjectType = O;

    template <size_t I>
    using ArgType = std::tuple_element_t<I, std::tuple<A...>>;

    static constexpr size_t getArgCount() { return sizeof...(A); }
};

template <typename T>
struct FunctorInfo : FunctorInfo<decltype(&T::operator())>
{
};

template <typename R, typename... A>
struct FunctorInfo<R (*)(A...)> : FunctorTrait<R, void, A...>
{
};

template <typename R, typename... A>
struct FunctorInfo<R(A...)> : FunctorTrait<R, void, A...>
{
};

template <typename R, typename O, typename... A>
struct FunctorInfo<R (O::*)(A...) const> : FunctorTrait<R, O, A...>
{
};

template <typename R, typename O, typename... A>
struct FunctorInfo<R (O::*)(A...)> : FunctorTrait<R, O, A...>
{
};

template <typename T, size_t I>
using GetArgType = typename FunctorInfo<T>::template ArgType<I>;

template <typename T, size_t I>
using DecayArgType = std::decay_t<GetArgType<T, I>>;
} // namespace brayns