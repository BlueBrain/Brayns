/* Copyright (c) 2021 EPFL/Blue Brain Project
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

#include "Message.h"

#include <brayns/common/utils/stringUtils.h>

#include <brayns/pluginapi/PluginAPI.h>

namespace brayns
{
struct NoMessageObject
{
};

template <typename MessageType>
struct MessageObject
{
    using Type = NoMessageObject;

    static std::string getName() { return {}; }

    static Type* extract(PluginAPI& api) { return nullptr; }

    static bool dump(const Type& object, MessageType& message)
    {
        return true;
    }

    static bool load(const MessageType& message, Type& object)
    {
        return true;
    }
};

struct MessageObjectName
{
    template<typename T>
    static std::string hyphen()
    {
        auto name = MessageObject<T>::getName();
        return string_utils::camelCaseToSeparated(name, '-');
    }

    template<typename T>
    static std::string spaced()
    {
        auto name = MessageObject<T>::getName();
        return string_utils::camelCaseToSeparated(name, ' ');
    }
};

struct MessageExtractor
{
    template <typename T>
    static bool extract(PluginAPI& api, T& message)
    {
        auto object = MessageObject<T>::extract(api);
        if (!object)
        {
            return false;
        }
        return MessageObject<T>::dump(*object, message);
    }

    template <typename T>
    static bool load(const T& message, PluginAPI& api)
    {
        auto object = MessageObject<T>::extract(api);
        if (!object)
        {
            return false;
        }
        return MessageObject<T>::load(message, *object);
    }
};
} // namespace brayns