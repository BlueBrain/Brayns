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

#include <brayns/engine/model/Components.h>

#include <api/coloring/IColorHandler.h>
#include <api/neuron/NeuronColorMethod.h>

class SomaColorHandler final : public IColorHandler
{
public:
    SomaColorHandler(brayns::Components &components);

    void updateColor(const brayns::Vector4f &color) override;

    std::vector<uint64_t> updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) override;

    void updateColorById(const std::vector<brayns::Vector4f> &colors) override;

    void updateColorByMethod(
        const IColorData &colorData,
        const std::string &method,
        const std::vector<ColoringInformation> &vars) override;

    void updateIndexedColor(const std::vector<brayns::Vector4f> &color, const std::vector<uint8_t> &indices) override;

private:
    void _colorWithInput(
        const IColorData &colorData,
        const std::string &method,
        const std::vector<ColoringInformation> &vars);

    void _colorAll(const IColorData &colorData, const std::string &method);

private:
    brayns::Components &_components;
};
