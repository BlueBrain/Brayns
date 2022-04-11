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

#include <brayns/engine/Model.h>

#include <api/coloring/IColorHandler.h>
#include <api/neuron/NeuronColorMethods.h>
#include <components/MorphologyCircuitComponent.h>

/**
 * @brief The MorphologyColorHandler class is the handler interface implementation to update colors on
 * morphology circuits
 */
class MorphologyColorHandler final : public IColorHandler
{
public:
    MorphologyColorHandler(MorphologyCircuitComponent &circuit);

    /**
     * @brief updateColor
     * @param color
     */
    void updateColor(const brayns::Vector4f &color) override;

    /**
     * @brief updateColorById
     * @param colorMap
     */
    void updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) override;

    /**
     * @brief updateColorById
     * @param colors
     */
    void updateColorById(const std::vector<brayns::Vector4f> &colors) override;

    /**
     * @brief updateColor
     * @param method
     * @param vars
     */
    void updateColorByMethod(
        const IColorData &colorData,
        const std::string &method,
        const std::vector<ColoringInformation> &vars) override;

    /**
     * @brief updateIndexedColor
     * @param color
     * @param indices
     */
    void updateIndexedColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices) override;

private:
    void _colorWithInput(
        const IColorData &colorData,
        const std::string &method,
        const std::vector<ColoringInformation> &vars);

    void _colorAll(const IColorData &colorData, const std::string &method);

private:
    MorphologyCircuitComponent &_circuit;
};