/* Copyright (c) 2020, EPFL/Blue Brain Project
 *
 * Responsible Author: Nadir Román Guerrero
 *                              <nadir.romanguerrero@epfl.ch>
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

#ifndef BRAYNS_COMMON_ACTIONMESSAGE_H
#define BRAYNS_COMMON_ACTIONMESSAGE_H

#include <brayns/common/propertymap/PropertyMap.h>

#include <type_traits>

namespace brayns
{
/**
 * Class used to add the property to the property map, so it is
 * always present as soon as the message is created
 */
template <typename T, typename Name, typename Description>
struct MessageEntry
{
    template <typename = std::enable_if_t<
                  std::is_constructible<std::string, Name>::value &&
                  std::is_constructible<std::string, Description>::value>>
    MessageEntry(PropertyMap& map, Name&& name, Description&& descr)
    {
        if (!map.find(name))
            map.add({name, T(), {descr}});
    }
};

/**
 * @brief The MessageEntryRegisterer struct
 * Auxiliary class to register functions that allows to extract the value
 * from the map and to put them back
 */
struct MessageEntryRegisterer
{
    MessageEntryRegisterer(
        std::vector<std::function<void(void*, PropertyMap&)>>& toPropList,
        const std::function<void(void*, PropertyMap&)>& toPropMap,
        std::vector<std::function<void(void*, PropertyMap&)>>& fromPropList,
        const std::function<void(void*, PropertyMap&)>& fromPropMap)
    {
        toPropList.push_back(toPropMap);
        fromPropList.push_back(fromPropMap);
    }
};

#define MESSAGE_ENTRY(TYPE, NAME, DESCRIPTION)                         \
public:                                                                \
    TYPE NAME;                                                         \
                                                                       \
private:                                                               \
    brayns::MessageEntry<TYPE, decltype(#NAME), decltype(DESCRIPTION)> \
        NAME##Entry{_map, #NAME, DESCRIPTION};                         \
    brayns::MessageEntryRegisterer NAME##Registerer{                   \
        _toProp,                                                       \
        [](void* ptr, brayns::PropertyMap& map) {                      \
            MsgType* msg = static_cast<MsgType*>(ptr);                 \
            map.update(#NAME, msg->NAME);                              \
        },                                                             \
        _fromProp,                                                     \
        [](void* ptr, brayns::PropertyMap& map) {                      \
            MsgType* msg = static_cast<MsgType*>(ptr);                 \
            msg->NAME = map[#NAME].as<TYPE>();                         \
        }};

struct Message
{
public:
    Message() {}
    Message(const PropertyMap& map)
        : _map(map)
    {
    }

    void fromPropertyMap()
    {
        for (const auto& fp : _fromProp)
            fp(this, _map);
    }

    void toPropertyMap()
    {
        for (const auto& tp : _toProp)
            tp(this, _map);
    }

    const PropertyMap& getPropertyMap() const { return _map; }

    template <typename Message,
              typename = std::enable_if_t<
                  std::is_constructible<std::string, Message>::value>>
    void setError(int32_t code, Message&& message)
    {
        if (!_map.find("error") && !_map.find("message"))
        {
            Property error{"error", code, {""}};
            error.setReadOnly(true);
            Property errorMessage{"message", std::string(message), {}};
            errorMessage.setReadOnly(true);
            _map.add(error);
            _map.add(errorMessage);
        }
    }

protected:
    PropertyMap _map;
    std::vector<std::function<void(void*, PropertyMap&)>> _toProp;
    std::vector<std::function<void(void*, PropertyMap&)>> _fromProp;
};

#define MESSAGE_BEGIN(CLASS)              \
public:                                   \
    CLASS()                               \
        : brayns::Message()               \
    {                                     \
    }                                     \
    CLASS(const brayns::PropertyMap& map) \
        : brayns::Message(map)            \
    {                                     \
    }                                     \
    using MsgType = CLASS;

} // namespace brayns

#endif // BRAYNS_COMMON_ACTIONMESSAGE_H
