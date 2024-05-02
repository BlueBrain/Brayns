/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <brayns/core/Brayns.h>
#include <brayns/core/CommandLine.h>
#include <brayns/core/utils/Log.h>
#include <brayns/core/utils/Timer.h>

#ifdef BRAYNS_ENABLE_CIRCUITS
    #include <brayns/circuits/CircuitExplorerPlugin.h>
#endif

#ifdef BRAYNS_ENABLE_ATLAS
    #include <brayns/atlas/AtlasExplorerPlugin.h>
#endif

int main(int argc, const char **argv)
{
    try
    {
        auto commandLine = brayns::CommandLine(argc, argv);

        if (commandLine.hasVersion())
        {
            auto version = commandLine.getVersion();
            std::cout << version << '\n';
            return 0;
        }

        if (commandLine.hasHelp())
        {
            auto help = commandLine.getHelp();
            std::cout << help << '\n';
            return 0;
        }

        auto instance = brayns::Brayns(argc, argv);

#ifdef BRAYNS_ENABLE_CIRCUITS
        brayns::loadCircuitExplorer(instance);
#endif

#ifdef BRAYNS_ENABLE_ATLAS
        brayns::loadAtlasExplorer(instance);
#endif

        brayns::Log::info("Starting Brayns service.");

        auto timer = brayns::Timer();

        instance.runAsService();

        brayns::Log::info("Service was running for {} seconds.", timer.seconds());
    }
    catch (const std::exception &e)
    {
        brayns::Log::fatal("Fatal error: '{}'.", e.what());
        return 1;
    }
    catch (...)
    {
        brayns::Log::fatal("Unknown fatal error.");
        return 1;
    }

    return 0;
}