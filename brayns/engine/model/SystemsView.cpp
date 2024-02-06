/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
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

#include "SystemsView.h"

namespace brayns
{
SystemsView::SystemsView(Systems &systems, Components &components):
    _systems(&systems),
    _components(&components)
{
}

void SystemsView::update(const ParametersManager &parameters)
{
    _systems->_update ? _systems->_update->execute(parameters, *_components) : void();
}

InspectResultData SystemsView::inspect(const InspectContext &context)
{
    return _systems->_inspect ? _systems->_inspect->execute(context, *_components) : InspectResultData();
}

Bounds SystemsView::computeBounds(const TransformMatrix &matrix)
{
    return _systems->_bounds ? _systems->_bounds->compute(matrix, *_components) : Bounds();
}

std::vector<std::string> SystemsView::getColorMethods() const
{
    return _systems->_color ? _systems->_color->getMethods() : std::vector<std::string>();
}

std::vector<std::string> SystemsView::getColorValues(const std::string &method) const
{
    return _systems->_color ? _systems->_color->getValues(method, *_components) : std::vector<std::string>();
}

void SystemsView::applyColor(const std::string &method, const ColorMethodInput &input)
{
    _systems->_color ? _systems->_color->apply(method, input, *_components) : void();
}
}
