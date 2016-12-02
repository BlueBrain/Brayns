/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#ifndef ENGINEFACTORY_H
#define ENGINEFACTORY_H

#include <brayns/common/types.h>

namespace brayns
{

/**
 * The engine factory is in charge of instanciating engines according to their
 * name (ospray, optix or firerays). If Brayns does not find the 3rd party
 * library at compilation time, the according lib is not generated and the
 * get method returns a null pointer.
 */
class EngineFactory
{
public:
    /**
     * @brief Default constructor
     * @param argc Number of command line arguments
     * @param argv Command line arguments
     * @param parametersManager Container for all parameters (application,
     *        rendering, geometry, scene)
     */
    EngineFactory(
        int argc,
        const char **argv,
        ParametersManager& parametersManager );
    ~EngineFactory() {}

    /**
     * @brief Gets the instance of the engine corresponding the given name. If
     *        the instance does not exist, it is created. If the name is
     *        incorrect, a null pointer is returned.
     * @param name of the engine (opray, optix, firerays)
     * @return A pointer to the engine, null if the engine could not be
     *         instanciated
     */
    EnginePtr get( const std::string& name );

private:

    strings _arguments;
    ParametersManager& _parametersManager;
    EngineMap _engines;
};

}

#endif // ENGINEFACTORY_H
