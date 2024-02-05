/* Copyright 2015-2024 Blue Brain Project/EPFL
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman <nadir.romanguerrero@epfl.ch>
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

#include "CircuitColorHandler.h"

#include <brayns/engine/Material.h>
#include <brayns/utils/StringUtils.h>

#include <plugin/api/ColorUtils.h>

#include <numeric>

namespace
{
std::vector<uint64_t> parseIDRanges(const std::string& input)
{
    if (input.empty())
        throw IDRangeParseException("Empty ID or ID range");

    std::vector<uint64_t> result;
    const auto dashPos = input.find("-");
    if (dashPos == std::string::npos)
    {
        try
        {
            result.push_back(std::stoull(input));
        }
        catch (...)
        {
            throw IDRangeParseException("Cannot parse ID '" + input + "'");
        }
    }
    else
    {
        const auto rangeBeginStr = input.substr(0, dashPos);
        const auto rangeEndStr = input.substr(dashPos + 1);
        try
        {
            const auto rangeStart = std::stoull(rangeBeginStr);
            const auto rangeEnd = std::stoull(rangeEndStr) + 1;

            if (rangeEnd <= rangeStart)
                throw IDRangeParseException(
                    "The range end must be greater than the range "
                    "start for: '" +
                    input + "'");
            result.resize(rangeEnd - rangeStart);
            std::iota(result.begin(), result.end(), rangeStart);
        }
        catch (...)
        {
            throw IDRangeParseException("Could not parse ID range '" + input +
                                        "'");
        }
    }

    return result;
}

void updateMaterialImpl(brayns::ModelDescriptor* model, const size_t id,
                        const brayns::Vector4f& color)
{
    if (id == std::numeric_limits<size_t>::max())
        return;

    auto material = model->getModel().getMaterial(id);
    material->setDiffuseColor(brayns::Vector3d(color.r, color.g, color.b));
    material->setOpacity(static_cast<double>(color.a));
    material->markModified();
    material->commit();
}

} // namespace

void ElementMaterialMap::_updateMaterial(brayns::ModelDescriptor* model,
                                         const size_t id,
                                         const brayns::Vector4f& color) const
{
    updateMaterialImpl(model, id, color);
}

void CircuitColorHandler::initialize()
{
    _methods = _getMethodsImpl();
    _methodVariables.resize(_methods.size());
}

void CircuitColorHandler::setElements(
    const std::vector<uint64_t>& ids,
    std::vector<ElementMaterialMap::Ptr>&& elements)
{
    _setElementsImpl(ids, std::move(elements));
}

const std::vector<std::string>& CircuitColorHandler::getMethods() const noexcept
{
    return _methods;
}

const std::vector<std::string>& CircuitColorHandler::getMethodVariables(
    const std::string& method) const
{
    const auto lcm = brayns::string_utils::toLowercase(method);

    for (size_t i = 0; i < _methods.size(); ++i)
    {
        if (lcm == _methods[i])
        {
            auto& cache = _methodVariables[i];
            if (!cache.initialized)
            {
                cache.variables = _getMethodVariablesImpl(_methods[i]);
                cache.initialized = true;
            }
            return cache.variables;
        }
    }

    throw ColorMethodNotFoundException();
}

void CircuitColorHandler::updateColorById(
    const std::vector<ColoringInformation>& vars)
{
    std::map<uint64_t, brayns::Vector4f> colorMap;
    if (!vars.empty())
    {
        for (const auto& entry : vars)
        {
            const auto& rawIds = entry.variable;
            const auto& color = entry.color;

            const auto ids = parseIDRanges(rawIds);
            for (const auto id : ids)
                colorMap[id] = color;
        }
    }

    _updateColorByIdImpl(colorMap);
    _model->markModified();
}

void CircuitColorHandler::updateColorById(
    const std::map<uint64_t, brayns::Vector4f>& colorMap)
{
    _updateColorByIdImpl(colorMap);
    _model->markModified();
}

void CircuitColorHandler::updateSingleColor(const brayns::Vector4f& color)
{
    _updateSingleColorImpl(color);
    _model->markModified();
}

void CircuitColorHandler::updateColor(
    const std::string& method, const std::vector<ColoringInformation>& vars)
{
    const auto lcm = brayns::string_utils::toLowercase(method);

    auto it = std::find(_methods.begin(), _methods.end(), lcm);
    if (it == _methods.end())
        throw ColorMethodNotFoundException();

    if (!vars.empty())
    {
        const auto& methodVars = getMethodVariables(method);
        for (const auto& var : vars)
        {
            auto it =
                std::find(methodVars.begin(), methodVars.end(), var.variable);
            if (it == methodVars.end())
                throw ColorMethodVariableNotFoundException();
        }
    }

    _updateColorImpl(lcm, vars);
    _model->markModified();
}

size_t CircuitColorHandler::getModelID() const noexcept
{
    return _model->getModelID();
}

void CircuitColorHandler::_updateMaterial(const size_t id,
                                          const brayns::Vector4f& color)
{
    updateMaterialImpl(_model, id, color);
}

void CircuitColorHandler::setModel(brayns::ModelDescriptor* model)
{
    _model = model;
}
