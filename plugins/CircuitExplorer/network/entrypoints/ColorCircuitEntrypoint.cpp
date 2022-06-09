/* Copyright (c) 2015-2022 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: nadir.romanguerrero@epfl.ch
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

#include "ColorCircuitEntrypoint.h"

#include <brayns/network/common/ExtractModel.h>

#include <api/coloring/ColorIDParser.h>
#include <components/CircuitColorComponent.h>

namespace
{
struct ColorHandlerExtractor
{
    static IColorHandler &extract(brayns::Scene &scene, const uint32_t modelId)
    {
        auto &modelInstance = brayns::ExtractModel::fromId(scene, modelId);
        auto &model = modelInstance.getModel();
        auto &colorComponent = model.getComponent<CircuitColorComponent>();
        auto &colorHandler = colorComponent.getColorHandler();
        return colorHandler;
    }
};

struct ColorDataExtractor
{
    static IColorData &extract(brayns::Scene &scene, const uint32_t modelId)
    {
        auto &modelInstance = brayns::ExtractModel::fromId(scene, modelId);
        auto &model = modelInstance.getModel();
        auto &colorComponent = model.getComponent<CircuitColorComponent>();
        auto &colorData = colorComponent.getColorData();
        return colorData;
    }
};
}

ColorCircuitByIdEntrypoint::ColorCircuitByIdEntrypoint(brayns::Scene &scene)
    : _scene(scene)
{
}

std::string ColorCircuitByIdEntrypoint::getMethod() const
{
    return "color-circuit-by-id";
}

std::string ColorCircuitByIdEntrypoint::getDescription() const
{
    return "Colors a circuit model by element ID. Specific IDs can be targeted, "
           "otherwise, random colors per ID will be applied. Returns a list of IDs that were not colored (if any)";
}

void ColorCircuitByIdEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    const auto modelId = params.model_id;
    const auto &colorInfo = params.color_info;
    auto &colorHandler = ColorHandlerExtractor::extract(_scene, modelId);
    const auto colorMap = ColorIDParser::parse(colorInfo);

    auto nonColored = colorHandler.updateColorById(colorMap);

    request.reply(nonColored);
}

ColorCircuitBySingleColorEntrypoint::ColorCircuitBySingleColorEntrypoint(brayns::Scene &scene)
    : _scene(scene)
{
}

std::string ColorCircuitBySingleColorEntrypoint::getMethod() const
{
    return "color-circuit-by-single-color";
}

std::string ColorCircuitBySingleColorEntrypoint::getDescription() const
{
    return "Colors a whole circuit model with a single color";
}

void ColorCircuitBySingleColorEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    const auto modelId = params.model_id;
    const auto &color = params.color;
    auto &colorHandler = ColorHandlerExtractor::extract(_scene, modelId);
    colorHandler.updateColor(color);
    request.reply(brayns::EmptyMessage());
}

AvailableColorMethodsEntrypoint::AvailableColorMethodsEntrypoint(brayns::Scene &scene)
    : _scene(scene)
{
}

std::string AvailableColorMethodsEntrypoint::getMethod() const
{
    return "get-circuit-color-methods";
}

std::string AvailableColorMethodsEntrypoint::getDescription() const
{
    return "Return the available extra coloring methods for a circuit model";
}

void AvailableColorMethodsEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    const auto modelId = params.model_id;
    auto &colorData = ColorDataExtractor::extract(_scene, modelId);
    request.reply({colorData.getMethods()});
}

AvailableColorMethodVariablesEntrypoint::AvailableColorMethodVariablesEntrypoint(brayns::Scene &scene)
    : _scene(scene)
{
}

std::string AvailableColorMethodVariablesEntrypoint::getMethod() const
{
    return "get-circuit-color-method-variables";
}

std::string AvailableColorMethodVariablesEntrypoint::getDescription() const
{
    return "Return the available variables which can be specified when coloring a circuit model by the given method";
}

void AvailableColorMethodVariablesEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    const auto modelId = params.model_id;
    const auto &method = params.method;
    auto &colorData = ColorDataExtractor::extract(_scene, modelId);
    request.reply({colorData.getMethodVariables(method)});
}

ColorCircuitByMethodEntrypoint::ColorCircuitByMethodEntrypoint(brayns::Scene &scene)
    : _scene(scene)
{
}

std::string ColorCircuitByMethodEntrypoint::getMethod() const
{
    return "color-circuit-by-method";
}

std::string ColorCircuitByMethodEntrypoint::getDescription() const
{
    return "Colors a circuit model by grouping its elements using the given method. "
           "Specific variables can be targetted, otherwise, random colors per variable group will be applied";
}

void ColorCircuitByMethodEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    const auto modelId = params.model_id;
    const auto &method = params.method;
    const auto &variables = params.color_info;
    auto &colorData = ColorDataExtractor::extract(_scene, modelId);
    auto &colorHandler = ColorHandlerExtractor::extract(_scene, modelId);
    colorHandler.updateColorByMethod(colorData, method, variables);
    request.reply(brayns::EmptyMessage());
}
