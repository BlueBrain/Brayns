/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
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

#include "ModelColoringEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

namespace brayns
{
GetColorMethodsEntrypoint::GetColorMethodsEntrypoint(ModelManager &models):
    _models(models)
{
}

std::string GetColorMethodsEntrypoint::getMethod() const
{
    return "get-color-methods";
}

std::string GetColorMethodsEntrypoint::getDescription() const
{
    return "Returns a list of available coloring methods for the model";
}

void GetColorMethodsEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &instance = ExtractModel::fromId(_models, params.id);
    auto &model = instance.getModel();
    auto systemView = model.getSystemsView();
    request.reply(systemView.getColorMethods());
}

GetColorValuesEntrypoint::GetColorValuesEntrypoint(ModelManager &models):
    _models(models)
{
}

std::string GetColorValuesEntrypoint::getMethod() const
{
    return "get-color-values";
}

std::string GetColorValuesEntrypoint::getDescription() const
{
    return "Returns a list of input variables for a given model and color method";
}

void GetColorValuesEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    auto &instance = ExtractModel::fromId(_models, params.id);
    auto &model = instance.getModel();
    auto systemView = model.getSystemsView();
    request.reply(systemView.getColorValues(params.method));
}

ColorModelEntrypoint::ColorModelEntrypoint(ModelManager &models):
    _models(models)
{
}

std::string ColorModelEntrypoint::getMethod() const
{
    return "color-model";
}

std::string ColorModelEntrypoint::getDescription() const
{
    return "Applies the specified color method to the model with the given color input";
}

void ColorModelEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();

    if (params.values.empty())
    {
        throw InvalidParamsException("Color values cannot be empty");
    }

    auto &instance = ExtractModel::fromId(_models, params.id);
    auto &model = instance.getModel();
    auto systemView = model.getSystemsView();
    systemView.applyColor(params.method, params.values);
    request.reply(EmptyJson());
}
}
