/* Copyright (c) 2015-2021 EPFL/Blue Brain Project
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

ColorCircuitByIdEntrypoint::ColorCircuitByIdEntrypoint(CircuitColorManager &manager)
    : _manager(manager)
{
}

std::string ColorCircuitByIdEntrypoint::getName() const
{
    return "color-circuit-by-id";
}

std::string ColorCircuitByIdEntrypoint::getDescription() const
{
    return "Colors a circuit model by element ID. "
           "Specific IDs can be targetted as single IDs or ID Ranges (being-end)."
           "Otherwise, random colors per ID will be applied";
}

void ColorCircuitByIdEntrypoint::onRequest(const Request &request)
{
    auto params = request.getParams();
    try
    {
        auto &scene = getApi().getScene();
        const auto &descriptor = brayns::ExtractModel::fromId(scene, params.model_id);
        _manager.updateColorsById(descriptor, params.color_info);
        scene.markModified();
        getApi().triggerRender();
    }
    catch (const CircuitModelNotFoundException &)
    {
        throw brayns::EntrypointException(1, "The given ID does not correspond to any circuit model");
    }
    catch (const IDRangeParseException &irpe)
    {
        throw brayns::EntrypointException(2, irpe.what());
    }

    request.reply(brayns::EmptyMessage());
}

ColorCircuitBySingleColorEntrypoint::ColorCircuitBySingleColorEntrypoint(CircuitColorManager &manager)
    : _manager(manager)
{
}

std::string ColorCircuitBySingleColorEntrypoint::getName() const
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
    try
    {
        auto &scene = getApi().getScene();
        const auto &descriptor = brayns::ExtractModel::fromId(scene, params.model_id);
        _manager.updateSingleColor(descriptor, params.color);
        scene.markModified();
        getApi().triggerRender();
    }
    catch (const CircuitModelNotFoundException &)
    {
        throw brayns::EntrypointException(1, "The given ID does not correspond to any circuit model");
    }
    request.reply(brayns::EmptyMessage());
}

AvailableColorMethodsEntrypoint::AvailableColorMethodsEntrypoint(CircuitColorManager &manager)
    : _manager(manager)
{
}

std::string AvailableColorMethodsEntrypoint::getName() const
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
    auto &scene = getApi().getScene();
    const auto &descriptor = brayns::ExtractModel::fromId(scene, params.model_id);
    try
    {
        request.reply({_manager.getAvailableMethods(descriptor)});
    }
    catch (const CircuitModelNotFoundException &)
    {
        throw brayns::EntrypointException(1, "The given ID does not correspond to any circuit model");
    }
}

AvailableColorMethodVariablesEntrypoint::AvailableColorMethodVariablesEntrypoint(CircuitColorManager &manager)
    : _manager(manager)
{
}

std::string AvailableColorMethodVariablesEntrypoint::getName() const
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
    auto &scene = getApi().getScene();
    const auto &descriptor = brayns::ExtractModel::fromId(scene, params.model_id);
    try
    {
        request.reply({_manager.getMethodVariables(descriptor, params.method)});
    }
    catch (const CircuitModelNotFoundException &)
    {
        throw brayns::EntrypointException(1, "The given ID does not correspond to any circuit model");
    }
    catch (const ColorMethodNotFoundException &)
    {
        throw brayns::EntrypointException(2, "The given coloring method does not exists for the given model");
    }
}

ColorCircuitByMethodEntrypoint::ColorCircuitByMethodEntrypoint(CircuitColorManager &manager)
    : _manager(manager)
{
}

std::string ColorCircuitByMethodEntrypoint::getName() const
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
    auto &scene = getApi().getScene();
    const auto &descriptor = brayns::ExtractModel::fromId(scene, params.model_id);
    try
    {
        _manager.updateColors(descriptor, params.method, params.color_info);
        scene.markModified();
        getApi().triggerRender();
    }
    catch (const CircuitModelNotFoundException &)
    {
        throw brayns::EntrypointException(1, "The given ID does not correspond to any circuit model");
    }
    catch (const ColorMethodNotFoundException &)
    {
        throw brayns::EntrypointException(2, "The given coloring method does not exists for the given model");
    }
    catch (const ColorMethodVariableNotFoundException &)
    {
        throw brayns::EntrypointException(3, "One or more method variables specified does not exist");
    }

    request.reply(brayns::EmptyMessage());
}
