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

#include "JsonValue.h"

#include <sstream>

#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Stringifier.h>

namespace brayns::experimental
{
JsonArray::Ptr createJsonArray()
{
    return Poco::makeShared<JsonArray>();
}

JsonObject::Ptr createJsonObject()
{
    return Poco::makeShared<JsonObject>();
}

bool isArray(const JsonValue &json)
{
    return json.type() == typeid(JsonArray::Ptr);
}

bool isObject(const JsonValue &json)
{
    return json.type() == typeid(JsonObject::Ptr);
}

const JsonArray &getArray(const JsonValue &json)
{
    try
    {
        return *json.extract<JsonArray::Ptr>();
    }
    catch (const Poco::Exception &e)
    {
        throw JsonException(e.displayText());
    }
}

const JsonObject &getObject(const JsonValue &json)
{
    try
    {
        return *json.extract<JsonObject::Ptr>();
    }
    catch (const Poco::Exception &e)
    {
        throw JsonException(e.displayText());
    }
}

std::string stringify(const JsonValue &json)
{
    auto stream = std::ostringstream();
    Poco::JSON::Stringifier::condense(json, stream, 0);
    return stream.str();
}

JsonValue parseJson(const std::string &data)
{
    try
    {
        auto parser = Poco::JSON::Parser();
        return parser.parse(data);
    }
    catch (const Poco::Exception &e)
    {
        throw JsonException(e.displayText());
    }
}
}
