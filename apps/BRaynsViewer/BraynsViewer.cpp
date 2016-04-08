/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of BRayns
 */

#include "BraynsViewer.h"

#include <brayns/Brayns.h>
#include <brayns/common/log.h>
#include <brayns/common/parameters/ParametersManager.h>
#include <brayns/common/scene/Scene.h>

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
    case ']':
        _brayns->getScene( ).setTimestamp(
            _brayns->getScene( ).getTimestamp( ) + _timestampIncrement);
        BRAYNS_INFO << "Timestamp: " <<
            _brayns->getScene( ).getTimestamp( ) << std::endl;
        break;
    case '[':
        _brayns->getScene( ).setTimestamp(
            _brayns->getScene( ).getTimestamp( ) - _timestampIncrement);
        BRAYNS_INFO << "Timestamp: " <<
            _brayns->getScene( ).getTimestamp( ) << std::endl;
        break;
    case '*':
        _brayns->getParametersManager( ).printHelp( );
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
        _brayns->getScene( ).setTimestamp(
            _brayns->getScene( ).getTimestamp( ) + _timestampIncrement );
        _brayns->commit( );
    }

    BaseWindow::display( );

    std::stringstream ss;
    ss << "BRayns Viewer - Interactive Ray-Tracing";
    if( _brayns->getParametersManager().getApplicationParameters( ).
        isBenchmarking( ))
    {
        ss << "@ " << _fps.getFPS( );
    }
    setTitle(ss.str( ));
    forceRedraw( );
}

}
