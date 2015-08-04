/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
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

#include <brayns/common/parameters/RenderingParameters.h>
#include <brayns/common/parameters/GeometryParameters.h>
#include <brayns/common/parameters/ApplicationParameters.h>
#include <brayns/common/log.h>
#include "BraynsViewer.h"

brayns::BraynsViewer* braynsViewer = NULL;

/** Define the function to be called when ctrl-c (SIGINT) signal is sent
 * to process
 */
void signal_callback_handler(int signum)
{
    switch( signum )
    {
    case SIGTERM:
        BRAYNS_INFO << "Gently closing application" << std::endl;
        delete braynsViewer;
        break;
    }
    exit(signum);
}

const ospray::vec3f DEFAULT_ENV_SCALE = {5,0.5,5};

int main(int argc, const char **argv)
{
    signal(SIGTERM, signal_callback_handler);

    brayns::ApplicationParameters applicationParameters;
    brayns::RenderingParameters renderingParameters;
    brayns::GeometryParameters geometryParameters;

    BRAYNS_INFO << "Parsing parameters..." << std::endl;
    applicationParameters.parse(argc, argv);
    applicationParameters.display();

    renderingParameters.parse(argc, argv);
    renderingParameters.display();

    geometryParameters.parse(argc, argv);
    geometryParameters.display();

    BRAYNS_INFO << "Initializing OSPRay..." << std::endl;
    ospInit(&argc,argv);
    if( applicationParameters.getModule() != "" )
        ospLoadModule(applicationParameters.getModule().c_str());

    brayns::initGLUT(&argc,argv);
    braynsViewer = new brayns::BraynsViewer( applicationParameters );
    if(braynsViewer)
    {
        BRAYNS_INFO << "Initializing Application..." << std::endl;
        braynsViewer->create( "BRayns Viewer",
            applicationParameters.getWindowWidth(),
            applicationParameters.getWindowHeight(), false);
        braynsViewer->setRenderingParameters(renderingParameters);
        braynsViewer->setGeometryParameters(geometryParameters);

        BRAYNS_INFO << "Loading data..." << std::endl;
        braynsViewer->loadData();
        braynsViewer->buildEnvironment( DEFAULT_ENV_SCALE );

        BRAYNS_INFO << "Building Geometry" << std::endl;
        braynsViewer->buildGeometry();
        brayns::runGLUT();
        delete braynsViewer;
    }
    else
    {
        BRAYNS_ERROR << "Failed to create braynsViewer" << std::endl;
    }
}
