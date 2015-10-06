/* Copyright (c) 2011-2014, EPFL/Blue Brain Project
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include <signal.h>

#include <brayns/common/loaders/MorphologyLoader.h>
#include <brayns/common/parameters/ApplicationParameters.h>
#include <brayns/common/log.h>

#include "BraynsService.h"

#ifndef __APPLE__
// GCC automtically removes the library if the application does not
// make an explicit use of one of its classes. In the case of OSPRay
// classes are loaded dynamicaly. The following line is only to make
// sure that the hbpKernel library is loaded.
#include <brayns/kernels/render/ExtendedOBJRenderer.h>
brayns::ExtendedOBJRenderer __r__;
#endif

int main(int argc, const char **argv)
{
    brayns::ApplicationParameters applicationParameters;
    brayns::RenderingParameters renderingParameters;
    brayns::GeometryParameters geometryParameters;

    applicationParameters.parse(argc, argv);
    applicationParameters.display();

    renderingParameters.parse(argc,argv);
    renderingParameters.display();

    geometryParameters.parse(argc,argv);
    geometryParameters.display();

    BRAYNS_INFO << "Initializing OSPRay..." << std::endl;
    ospInit(&argc,argv);
    if( applicationParameters.getModule() != "" )
        ospLoadModule(applicationParameters.getModule().c_str());

    BRAYNS_INFO << "Initializing Service..." << std::endl;
    brayns::BraynsService braynsService(applicationParameters);
    braynsService.setRenderingParameters(renderingParameters);
    braynsService.setGeometryParameters(geometryParameters);

    BRAYNS_INFO << "Loading data" << std::endl;
    braynsService.loadData();
    BRAYNS_INFO << "Building Geometry" << std::endl;
    braynsService.buildGeometry();
    BRAYNS_INFO << "Processing events..." << std::endl;
    braynsService.serveEvents(true);
    while( braynsService.isServingEvents() )
    {
        braynsService.processEvents();
    }
    return 0;
}
