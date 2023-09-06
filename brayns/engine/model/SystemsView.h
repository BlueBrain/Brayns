/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
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

#include "Components.h"
#include "Systems.h"

namespace brayns
{
class SystemsView
{
public:
    SystemsView(Systems &systems, Components &components);

    void update(const ParametersManager &parameters);
    InspectResultData inspect(const InspectContext &context);
    Bounds computeBounds(const TransformMatrix &matrix);
    std::vector<std::string> getColorMethods() const;
    std::vector<std::string> getColorValues(const std::string &method) const;
    void applyColor(const std::string &method, const ColorMethodInput &input);

private:
    Systems *_systems;
    Components *_components;
};
}
