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

#include "BraynsViewer.h"

#include <brayns/Brayns.h>
#include <brayns/common/log.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/common/scene/Scene.h>

namespace
{
    const float DEFAULT_TEST_TIMESTAMP = 10000.f;
}

namespace brayns
{

BraynsViewer::BraynsViewer(BraynsPtr brayns, int argc, const char **argv)
    : BaseWindow(brayns, argc, argv,
                 FRAMEBUFFER_COLOR,
                 INSPECT_CENTER_MODE,
                 INSPECT_CENTER_MODE|MOVE_MODE)
    , _timestampIncrement( 0.f )
{
}

void BraynsViewer::keypress(char key, const Vector2f& where)
{
    SceneParameters& sceneParams =
        _brayns->getParametersManager().getSceneParameters();
    RenderingParameters& renderingParams =
        _brayns->getParametersManager().getRenderingParameters();

    switch (key)
    {
    case '4':
        BRAYNS_INFO << "Setting gradient materials" << std::endl;
        _brayns->setMaterials(MT_GRADIENT);
        break;
    case '5':
        BRAYNS_INFO << "Setting pastel materials" << std::endl;
        _brayns->setMaterials(MT_PASTEL_COLORS);
        break;
    case '6':
        BRAYNS_INFO << "Setting random materials" << std::endl;
        _brayns->setMaterials(MT_RANDOM);
        break;
    case '7':
        BRAYNS_INFO << "Setting shades of grey materials" << std::endl;
        _brayns->setMaterials(MT_SHADES_OF_GREY);
        break;
    case 'g':
        _timestampIncrement = ( _timestampIncrement == 0.f ) ? 1.f : 0.f;
        BRAYNS_INFO << "Timestamp increment: " <<
            _timestampIncrement << std::endl;
        break;
    case 'x':
        sceneParams.setTimestamp( DEFAULT_TEST_TIMESTAMP );
        BRAYNS_INFO << "Test Timestamp" << std::endl;
        break;
    case ']':
        {
            float ts = sceneParams.getTimestamp( );
            sceneParams.setTimestamp( ts + 1 );
            BRAYNS_INFO << "Timestamp: " <<
                sceneParams.getTimestamp( ) << std::endl;
        }
        break;
    case '[':
        {
            float ts = sceneParams.getTimestamp( );
            if( ts > 0.f )
                sceneParams.setTimestamp( ts - 1 );
            BRAYNS_INFO << "Timestamp: " <<
                sceneParams.getTimestamp( ) << std::endl;
        }
        break;
    case '*':
        _brayns->getParametersManager( ).printHelp( );
        break;
    case '0':
        BRAYNS_INFO << "Default renderer activated" << std::endl;
        renderingParams.setRenderer("exobj");
        break;
    case '8':
        BRAYNS_INFO << "Touch detection renderer activated" << std::endl;
        renderingParams.setRenderer("proximityrenderer");
        break;
    case '9':
        BRAYNS_INFO << "Simulation renderer activated" << std::endl;
        renderingParams.setRenderer("simulationrenderer");
        break;
    default:
        BaseWindow::keypress(key,where);
    }
    _brayns->commit( );
}

void BraynsViewer::display( )
{
    if( _timestampIncrement != 0.f )
    {
        SceneParameters& sceneParams =
            _brayns->getParametersManager().getSceneParameters();
        sceneParams.setTimestamp(
            sceneParams.getTimestamp( ) + _timestampIncrement );
        _brayns->commit( );
    }

    BaseWindow::display( );

    std::stringstream ss;
    ss << "BRayns Viewer - Interactive Ray-Tracing";
    if( _brayns->getParametersManager().getApplicationParameters( ).
        isBenchmarking( ))
    {
        ss << " @ " << _fps.getFPS( );
    }
    setTitle(ss.str( ));
    forceRedraw( );
}

}
