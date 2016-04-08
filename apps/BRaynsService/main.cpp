/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include <brayns/common/types.h>
#include <brayns/common/log.h>
#include <brayns/Brayns.h>

int main(int argc, const char **argv)
{
    BRAYNS_INFO << "Initializing Service..." << std::endl;
    brayns::Brayns brayns(argc, argv);

    while( true )
        brayns.render( );

    return 0;
}
