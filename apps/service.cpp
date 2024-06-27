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

#include <iostream>

#include <brayns/core/Launcher.h>
#include <brayns/core/Version.h>
#include <brayns/core/cli/CommandLine.h>

using namespace brayns;

int main(int argc, const char **argv)
{
    try
    {
        auto settings = parseArgvAs<ServiceSettings>(argc, argv);

        if (settings.version)
        {
            std::cout << getCopyright() << '\n';
            return 0;
        }

        if (settings.help)
        {
            std::cout << getArgvHelp<ServiceSettings>() << '\n';
            return 0;
        }

        runService(settings);
    }
    catch (const std::exception &e)
    {
        std::cout << "Fatal error: " << e.what() << ".\n";
    }
    catch (...)
    {
        std::cout << "Unknown fatal error.";
        return 1;
    }

    return 0;
}
