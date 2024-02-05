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

#include "CircuitColorManager.h"

namespace
{
CircuitColorHandler* getHandler(
    const brayns::ModelDescriptor& model,
    const std::vector<std::unique_ptr<CircuitColorHandler>>& handlers)
{
    const auto it =
        std::find_if(handlers.begin(), handlers.end(),
                     [&](const CircuitColorHandler::Ptr& cch) {
                         return cch->getModelID() == model.getModelID();
                     });

    if (it == handlers.end())
        throw CircuitModelNotFoundException();

    return it->get();

    /*
    for (auto& handler : handlers)
    {
        if (handler->getModelID() == model.getModelID())
            return handler.get();
    }

    throw CircuitModelNotFoundException();
    */
}
} // namespace

void CircuitColorManager::registerHandler(
    brayns::ModelDescriptorPtr& model,
    std::unique_ptr<CircuitColorHandler>&& handler,
    const std::vector<uint64_t>& ids,
    std::vector<ElementMaterialMap::Ptr>&& elements)
{
    handler->setModel(model.get());
    handler->setElements(ids, std::move(elements));

    model->addOnRemoved([ccmPtr = this](const brayns::ModelDescriptor& model) {
        ccmPtr->unregisterHandler(model);
    });

    handler->initialize();
    _handlers.push_back(std::move(handler));
}

void CircuitColorManager::unregisterHandler(
    const brayns::ModelDescriptor& model)
{
    auto it = _handlers.begin();
    while (it != _handlers.end())
    {
        if ((*it)->getModelID() == model.getModelID())
        {
            _handlers.erase(it);
            break;
        }
        else
            ++it;
    }
}

bool CircuitColorManager::handlerExists(
    const brayns::ModelDescriptor& model) noexcept
{
    for (const auto& handler : _handlers)
    {
        if (handler->getModelID() == model.getModelID())
            return true;
    }

    return false;
}

const std::vector<std::string>& CircuitColorManager::getAvailableMethods(
    const brayns::ModelDescriptor& model)
{
    return getHandler(model, _handlers)->getMethods();
}

const std::vector<std::string>& CircuitColorManager::getMethodVariables(
    const brayns::ModelDescriptor& model, const std::string& method)
{
    return getHandler(model, _handlers)->getMethodVariables(method);
}

void CircuitColorManager::updateColorsById(
    const brayns::ModelDescriptor& model,
    const std::vector<ColoringInformation>& vars)
{
    getHandler(model, _handlers)->updateColorById(vars);
}

void CircuitColorManager::updateColorsById(
    const brayns::ModelDescriptor& model,
    const std::map<uint64_t, brayns::Vector4f>& colorMap)
{
    getHandler(model, _handlers)->updateColorById(colorMap);
}

void CircuitColorManager::updateSingleColor(
    const brayns::ModelDescriptor& model, const brayns::Vector4f& color)
{
    getHandler(model, _handlers)->updateSingleColor(color);
}

void CircuitColorManager::updateColors(
    const brayns::ModelDescriptor& model, const std::string& method,
    const std::vector<ColoringInformation>& vars)
{
    getHandler(model, _handlers)->updateColor(method, vars);
}
