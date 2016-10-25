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
#include <brayns/common/input/KeyboardHandler.h>

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
    KeyboardHandler& keyHandler = _brayns->getKeyboardHandler();
    keyHandler.registerKey( '4', "Set gradient materials" );
    keyHandler.registerKey( '5', "Set pastel materials" );
    keyHandler.registerKey( '6', "Set random materials" );
    keyHandler.registerKey( 'g', "Enable/Disable timestamp auto-increment" );
    keyHandler.registerKey( 'h', "Display shortcuts help" );
    keyHandler.registerKey( 'x', "Set timestamp to " + std::to_string( DEFAULT_TEST_TIMESTAMP ));
}

void BraynsViewer::keypress(char key, const Vector2f& where)
{
    SceneParameters& sceneParams =
        _brayns->getParametersManager().getSceneParameters();

    switch (key)
    {
    case '4':
        _brayns->setMaterials(MT_GRADIENT);
        break;
    case '5':
        _brayns->setMaterials(MT_PASTEL_COLORS);
        break;
    case '6':
        _brayns->setMaterials(MT_RANDOM);
        break;
    case 'g':
        _timestampIncrement = ( _timestampIncrement == 0.f ) ? 1.f : 0.f;
        break;
    case 'h':
        BRAYNS_INFO << _brayns->getKeyboardHandler().help() << std::endl;
        break;
    case 'x':
        sceneParams.setTimestamp( DEFAULT_TEST_TIMESTAMP );
        break;
    default:
        BaseWindow::keypress(key,where);
    }
    _brayns->getKeyboardHandler().logDescription( key );
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
    ss << "Brayns Viewer [" <<
          _brayns->getParametersManager().getRenderingParameters().getEngine() <<
          "] ";
    size_t ts = _brayns->getParametersManager().getSceneParameters().getTimestamp();
    if( ts != std::numeric_limits<size_t>::max() )
        ss << " (frame " << ts << ")";
    if( _brayns->getParametersManager().getApplicationParameters( ).
        isBenchmarking( ))
    {
        ss << " @ " << _fps.getFPS( );
    }
    setTitle(ss.str( ));
    forceRedraw( );
}

}
