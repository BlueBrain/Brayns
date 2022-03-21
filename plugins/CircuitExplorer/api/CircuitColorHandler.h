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
 * @brief The MethodVariableCache struct holds the information of a specific
 *        method variables, with a parameter to allow lazy caching
 */
struct MethodVariableCache
{
    bool initialized{false};
    std::vector<std::string> variables;
};

/**
 * @brief The ColoringInformation struct contains the color by which all the
 * elements of a circuit that matches the given variable for a given method must
 * be colored
 */
struct ColoringInformation
{
    std::string variable;
    brayns::Vector4f color;
};

/**
 * @brief The ColorDataReader is an abstract class to implement the functionality of fetching useful data
 * to color a neruonal/astrocyte/synapse/vasculature circuit depending on the format of the underlying disk
 * data (BBP, SONATA, ...)
 */
class ColorDataReader
{
public:
    virtual ~ColorDataReader() = default;

    /**
     * @brief getAvailableMethods Return the available methods by which a
     * circuit can be colored (For example: By ID, By layer, by population, ...)
     */
    virtual const std::vector<std::string> &getMethods() const noexcept = 0;

    /**
     * @brief getMethodVariables Return the possible variable specofications for
     * a given method (For example, for layer it will return the list of loaded
     * layers, for mtypes the list of loaded mtypes, ...)
     */
    virtual const std::vector<std::string> &getMethodVariables(const std::string &method) const = 0;
};
