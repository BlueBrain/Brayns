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

#include <brayns/core/engine/systems/GenericColorSystem.h>

#include <brayns/circuit/api/coloring/BrainColorMethod.h>

/**
 * @brief Color circuits based on data available on the circuit/nodes file.
 */
class BrainDatasetColorMethod : public brayns::IColorMethod
{
public:
    explicit BrainDatasetColorMethod(BrainColorMethod method);

    std::string getName() const override;
    std::vector<std::string> getValues(brayns::Components &components) const override;
    void apply(brayns::Components &components, const brayns::ColorMethodInput &input) const override;

private:
    BrainColorMethod _method;
};
