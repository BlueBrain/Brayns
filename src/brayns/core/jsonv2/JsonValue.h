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

#include <string>

#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/JSONException.h>
#include <Poco/JSON/Object.h>
#include <Poco/SharedPtr.h>

namespace brayns::experimental
{
using JsonValue = Poco::Dynamic::Var;
using JsonArray = Poco::JSON::Array;
using JsonObject = Poco::JSON::Object;
using JsonException = Poco::JSON::JSONException;

struct NullJson
{
};

JsonArray &createArray(JsonValue &json);
JsonObject &createObject(JsonValue &json);
bool isArray(const JsonValue &json);
bool isObject(const JsonValue &json);
const JsonArray &getArray(const JsonValue &json);
const JsonObject &getObject(const JsonValue &json);
std::string stringify(const JsonValue &json);
JsonValue parseJson(const std::string &data);
}
