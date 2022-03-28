/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#pragma once

#include <brayns/engine/Model.h>
#include <brayns/engine/common/DataHandler.h>

#include <map>
#include <stdexcept>
#include <unordered_map>
#include <vector>

/**
 * @brief Exception throw when parsing of ID and/or ID ranges fails
 */
class IDRangeParseException : public std::runtime_error
{
public:
    IDRangeParseException(const std::string &message);
};
/**
 * @brief Exception thrown when a requested method does not exists on a given
 * CircuitColorHandler
 */
class ColorMethodNotFoundException : public std::runtime_error
{
public:
    ColorMethodNotFoundException();
};
/**
 * @brief Exception thrown when a requested variable does not exists on a given
 * CircuitColorHandler and coloring method
 */
class ColorMethodVariableNotFoundException : public std::runtime_error
{
public:
    ColorMethodVariableNotFoundException();
};

/**
 * @brief The ColoringInformation matches a method variable with a color
 */
struct ColoringInformation
{
    std::string variable;
    brayns::Vector4f color;
};

/**
 * @brief The CircuitColorHandler class is the interface to implement coloring methods on different
 * types of geometry
 */
class CircuitColorHandler
{
public:
    virtual ~CircuitColorHandler() = default;

    /**
     * @brief updateColorById Updates color of the elements by the ID they are
     * identified by. Specific ids might be specified to isolate the update
     */
    virtual void updateColorById(const std::map<uint64_t, brayns::Vector4f> &colorMap) = 0;

    /**
     * @brief updateSingleColor Updates the color of all the elements to the
     * given color
     */
    virtual void updateSingleColor(const brayns::Vector4f &color) = 0;

    /**
     * @brief updateColor Updates the circuit color according to the given
     * method. If one or more variables are specified, only these will be
     * updated. Otherwise, updates the whole circuit.
     */
    virtual void updateColor(const std::string &method, const std::vector<ColoringInformation> &vars) = 0;

    /**
     * @brief updateSimulationColor udpates the circuit color to the simulation values
     * @param color
     * @param indices
     */
    virtual void updateSimulationColor(brayns::OSPBuffer &color, const std::vector<uint8_t> &indices) = 0;
};
