/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
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

#include <brayns/Brayns.h>
#include <brayns/common/types.h>
#include <brayns/common/log.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/parameters/ParametersManager.h>
#include "BraynsViewer.h"

int main(int argc, const char **argv)
{
    try
    {
        brayns::BraynsPtr brayns( new brayns::Brayns( argc, argv ));
        brayns::initGLUT( &argc, argv );
        brayns::BraynsViewer braynsViewer( brayns );
        BRAYNS_INFO << "Initializing Application..." << std::endl;
        const brayns::Vector2ui& size =
            brayns->getParametersManager().getApplicationParameters().getWindowSize();

        braynsViewer.create( "Brayns Viewer", size.x(), size.y(), false );
        brayns::runGLUT();
    }
    catch( const std::runtime_error& e )
    {
        BRAYNS_ERROR << e.what() << std::endl;
        return 1;
    }
    return 0;
}
