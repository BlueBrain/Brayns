/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include <string>

namespace brayns
{
class CommandLine
{
public:
    /**
     * @brief Get version display string.
     *
     * @return std::string Version display string.
     */
    static std::string getVersion();

    /**
     * @brief Get help display string.
     *
     * @return std::string Help display string.
     */
    static std::string getHelp();

    /**
     * @brief Construct with command line argv.
     *
     * @param argc Argc.
     * @param argv Argv.
     */
    CommandLine(int argc, const char **argv);

    /**
     * @brief Check if version print is required.
     *
     * @return true Version should be printed.
     * @return false Application can start normally.
     */
    bool hasVersion();

    /**
     * @brief Check if help print is required.
     *
     * @return true Help should be printed.
     * @return false Application can start normally.
     */
    bool hasHelp();

private:
    int _argc = 0;
    const char **_argv = nullptr;
};
} // namespace brayns
