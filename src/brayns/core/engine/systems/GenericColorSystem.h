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

#pragma once

#include <brayns/core/engine/model/systemtypes/ColorSystem.h>

#include <memory>
#include <vector>

namespace brayns
{
class IColorMethod
{
public:
    virtual ~IColorMethod() = default;

    virtual std::string getName() const = 0;
    virtual std::vector<std::string> getValues(Components &components) const = 0;
    virtual void apply(Components &components, const ColorMethodInput &input) const = 0;
};

using ColorMethodList = std::vector<std::unique_ptr<IColorMethod>>;

class GenericColorSystem : public ColorSystem
{
public:
    explicit GenericColorSystem(ColorMethodList methods);

    std::vector<std::string> getMethods() const override;
    std::vector<std::string> getValues(const std::string &method, Components &components) const override;
    void apply(const std::string &method, const ColorMethodInput &input, Components &components) const override;

private:
    ColorMethodList _methods;
    std::vector<std::string> _methodNames;
};
}
