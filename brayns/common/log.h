/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#ifndef BRAYNS_LOG_H
#define BRAYNS_LOG_H

#include <iostream>
#define BRAYNS_ERROR std::cerr << "[ERROR] "
#define BRAYNS_WARN std::cerr << "[WARN ] "
#define BRAYNS_INFO std::cout << "[INFO ] "
#ifdef NDEBUG
#  define BRAYNS_DEBUG if( false ) std::cout
#else
#  define BRAYNS_DEBUG std::cout << "[DEBUG] "
#endif

#define BRAYNS_THROW(exc) \
    {\
        BRAYNS_ERROR << exc.what() << std::endl;\
        throw exc;\
    }

#endif
