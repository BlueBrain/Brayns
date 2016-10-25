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

#ifndef KEYBOARDHANDLER_H
#define KEYBOARDHANDLER_H

#include <brayns/common/types.h>

namespace brayns
{

class KeyboardHandler
{

public:

    KeyboardHandler( ScenePtr scene, ParametersManagerPtr parametersManager );

    void processKey( const unsigned char key );

    std::string help();

    void registerKey( const unsigned char key, const std::string& description );
    void logDescription( const unsigned char key );

private:

    ParametersManagerPtr _parametersManager;
    ScenePtr _scene;
    std::map< unsigned char, std::string > _registeredKeys;

};

}

#endif // KEYBOARDHANDLER_H
