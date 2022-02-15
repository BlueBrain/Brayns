/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/engine/Model.h>
#include <brayns/json/Json.h>
#include <brayns/json/JsonObjectMacro.h>
#include <brayns/json/JsonSchemaValidator.h>
#include <brayns/utils/StringUtils.h>

namespace brayns
{
class IModelAction
{
public:
    virtual ~IModelAction() = default;

    virtual std::string_view getName() const noexcept = 0;

    virtual JsonValue executeAction(Model& model, const JsonValue& input) = 0;
};

template<typename Input, typename Output>
class ModelAction : public IModelAction
{
public:
    virtual ~ModelAction() = default;

    JsonValue executeAction(Model& model, const JsonValue& input) final
    {
        const auto schema = Json::getSchema<Input>();
        const auto errors = JsonSchemaValidator::validate(input, schema);
        if (!errors.empty())
        {
            const std::string name (getName());
            throw std::invalid_argument(
                "Could not parse " + name + " model action input: " + string_utils::join(errors, ", "));
        }

        Input inputParams;
        Json::deserialize<Input>(input, inputParams);

        const auto result = execute(model, inputParams);

        return Json::serialize(result);
    }

    virtual Output execute(Model& model, const Input& input) = 0;
};

BRAYNS_JSON_OBJECT_BEGIN(ModelActionEmptyParam)
BRAYNS_JSON_OBJECT_END()

template<typename Input>
class ModelActionCall : public ModelAction<Input, ModelActionEmptyParam>
{
public:
    ModelActionEmptyParam execute(Model& model, const Input& input)
    {
        call(model, input);
        return {};
    }

    virtual void call(Model& model, const Input& input) = 0;
};

template<typename Output>
class ModelActionRequeest : public ModelAction<ModelActionEmptyParam, Output>
{
public:
    Output execute(Model& model, ModelActionEmptyParam& input)
    {
        (void)input;
        return request(model);
    }

    virtual Output request(Model& model) = 0;
};
}
