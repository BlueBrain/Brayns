/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
 *
 * This file is part of BRayns
 */

#include <brayns/Brayns.h>
#include <brayns/common/types.h>
#include <brayns/common/log.h>
#include <brayns/parameters/ParametersManager.h>
#include <brayns/parameters/ParametersManager.h>
#include "BraynsViewer.h"

int main(int argc, const char **argv)
{
    brayns::BraynsPtr brayns( new brayns::Brayns( argc, argv ));
    brayns::initGLUT(&argc, argv);
    brayns::BraynsViewer braynsViewer(brayns, argc, argv);
    BRAYNS_INFO << "Initializing Application..." << std::endl;
    const brayns::Vector2ui& size =
        brayns->getParametersManager().getApplicationParameters().getWindowSize();

    braynsViewer.create( "BRayns Viewer", size.x(), size.y(), false);
    brayns::runGLUT();
}
