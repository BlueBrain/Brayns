/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
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

#include <apps/ui/Application.h>
#include <brayns/Brayns.h>
#include <brayns/common/log.h>

#include <brayns/parameters/ApplicationParameters.h>
#include <brayns/parameters/ParametersManager.h>

#include <cstdlib>

int main(int argc, const char** argv)
{
    try
    {
        brayns::Brayns brayns(argc, argv);
        auto appInstance = Application::createInstance(brayns);
        if (!appInstance->init())
        {
            Application::destroyInstance();
            return EXIT_FAILURE;
        }

        appInstance->run();
        Application::destroyInstance();
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_ERROR << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
