/* Copyright (c) 2011-2014, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of BRayns
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

#include "BraynsViewer.h"

#include <brayns/Brayns.h>
#include <brayns/common/log.h>
#include <brayns/common/parameters/ParametersManager.h>

namespace brayns
{

BraynsViewer::BraynsViewer(int argc, const char **argv)
    : BaseWindow(argc, argv,
                 FRAMEBUFFER_COLOR,
                 INSPECT_CENTER_MODE,
                 INSPECT_CENTER_MODE|MOVE_MODE)
{
}

void BraynsViewer::keypress(char key, const Vector2f& where)
{
    GeometryParameters& geoParams =
        _brayns->getParametersManager( ).getGeometryParameters( );
    RenderingParameters& renderParams =
        _brayns->getParametersManager( ).getRenderingParameters( );

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
        geoParams.setTimedGeometry( !geoParams.getTimedGeometry( ));
        BRAYNS_INFO << "Timed geometry: " <<
            ( geoParams.getTimedGeometry( ) ? "On" : "Off" ) << std::endl;
        break;
    case ']':
        renderParams.setFrameNumber(
            renderParams.getFrameNumber( ) +
            geoParams.getTimedGeometryIncrement( ));
        BRAYNS_INFO << "Frame number: " <<
            renderParams.getFrameNumber( ) << std::endl;
        break;
    case '[':
        renderParams.setFrameNumber(
            renderParams.getFrameNumber( ) -
            geoParams.getTimedGeometryIncrement( ));
        BRAYNS_INFO << "Frame number: " <<
            renderParams.getFrameNumber( ) << std::endl;
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
