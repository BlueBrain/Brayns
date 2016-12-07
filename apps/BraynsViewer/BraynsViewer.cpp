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
#include <brayns/common/engine/Engine.h>
#include <brayns/common/input/KeyboardHandler.h>

namespace
{
    const float DEFAULT_TEST_TIMESTAMP = 10000.f;
}

namespace brayns
{

BraynsViewer::BraynsViewer( Brayns& brayns )
    : BaseWindow( brayns )
    , _timestampIncrement( 0.f )
{
    _registerKeyboardShortcuts();
}

void BraynsViewer::_registerKeyboardShortcuts()
{
    BaseWindow::_registerKeyboardShortcuts();
    auto& keyHandler = _brayns.getKeyboardHandler();
    keyHandler.registerKeyboardShortcut(
        '3', "Set gradient materials",
        std::bind( &BraynsViewer::_gradientMaterials, this ));
    keyHandler.registerKeyboardShortcut(
        '4', "Set pastel materials",
        std::bind( &BraynsViewer::_pastelMaterials, this ));
    keyHandler.registerKeyboardShortcut(
        '5', "Set random materials",
        std::bind( &BraynsViewer::_randomMaterials, this ));
    keyHandler.registerKeyboardShortcut(
        'g', "Enable/Disable timestamp auto-increment",
         std::bind( &BraynsViewer::_toggleIncrementalTimestamp, this ));
    keyHandler.registerKeyboardShortcut(
        'x', "Set timestamp to " + std::to_string( DEFAULT_TEST_TIMESTAMP ),
        std::bind( &BraynsViewer::_defaultTimestamp, this ));
}

void BraynsViewer::_gradientMaterials()
{
    _brayns.getEngine().initializeMaterials( MT_GRADIENT );
}

void BraynsViewer::_pastelMaterials()
{
    _brayns.getEngine().initializeMaterials( MT_PASTEL_COLORS );
}

void BraynsViewer::_randomMaterials()
{
    _brayns.getEngine().initializeMaterials( MT_RANDOM );
}

void BraynsViewer::_toggleIncrementalTimestamp()
{
    _timestampIncrement = ( _timestampIncrement == 0.f ) ? 1.f : 0.f;
}

void BraynsViewer::_defaultTimestamp()
{
    auto& sceneParams = _brayns.getParametersManager().getSceneParameters();
    sceneParams.setTimestamp( DEFAULT_TEST_TIMESTAMP );
}

void BraynsViewer::display( )
{
    if( _timestampIncrement != 0.f )
    {
        auto& sceneParams = _brayns.getParametersManager().getSceneParameters();
        sceneParams.setTimestamp( sceneParams.getTimestamp( ) + _timestampIncrement );
        _brayns.getEngine().commit();
    }

    BaseWindow::display();

    std::stringstream ss;
    ss << "Brayns Viewer [" <<
          _brayns.getParametersManager().getRenderingParameters().getEngine() <<
          "] ";
    size_t ts = _brayns.getParametersManager().getSceneParameters().getTimestamp();
    if( ts != std::numeric_limits<size_t>::max() )
        ss << " (frame " << ts << ")";
    if( _brayns.getParametersManager().getApplicationParameters().isBenchmarking( ))
    {
        ss << " @ " << _fps.getFPS( );
    }
    setTitle(ss.str( ));
    forceRedraw( );
}

}
