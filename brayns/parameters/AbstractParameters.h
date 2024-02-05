/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
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

#include <brayns/common/BaseObject.h>

#include <boost/program_options.hpp>
#include <boost/program_options/value_semantic.hpp>

namespace po = boost::program_options;

namespace brayns
{
/**
   Base class defining command line parameters
 */
class AbstractParameters : public BaseObject
{
public:
    /**
       Constructor
       @param name Display name for the set of parameters
     */
    AbstractParameters(const std::string& name)
        : _name(name)
        , _parameters(name)
    {
    }

    virtual ~AbstractParameters() = default;
    /**
       Parses parameters managed by the class
       @param vm the variables map of all arguments passed by the user
     */
    virtual void parse(const po::variables_map&){};

    /**
       Displays values of registered parameters
     */
    virtual void print();

    po::options_description& parameters();

protected:
    std::string _name;

    po::options_description _parameters;

    static std::string asString(const bool flag) { return flag ? "on" : "off"; }
};
} // namespace brayns

namespace boost
{
namespace program_options
{
/**
 * Wrapper for supporting fixed size multitoken values
 */
template <typename T, typename charT = char>
class fixed_tokens_typed_value : public typed_value<T, charT>
{
    const unsigned _min;
    const unsigned _max;

    typedef typed_value<T, charT> base;

public:
    fixed_tokens_typed_value(T* t, unsigned min, unsigned max)
        : base(t)
        , _min(min)
        , _max(max)
    {
        base::multitoken();
    }
    unsigned min_tokens() const { return _min; }
    unsigned max_tokens() const { return _max; }
};

template <typename T>
inline fixed_tokens_typed_value<T>* fixed_tokens_value(unsigned min,
                                                       unsigned max)
{
    return new fixed_tokens_typed_value<T>(nullptr, min, max);
}

template <typename T>
inline fixed_tokens_typed_value<T>* fixed_tokens_value(T* t, unsigned min,
                                                       unsigned max)
{
    return new fixed_tokens_typed_value<T>(t, min, max);
}
} // namespace program_options
} // namespace boost
