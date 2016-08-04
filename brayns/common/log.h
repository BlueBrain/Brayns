/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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

#ifndef BRAYNS_LOG_H
#define BRAYNS_LOG_H

#include <iostream>
#include <mutex>
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

static std::mutex __logging_mtx;
#define BRAYNS_PROGRESS( __value, __maxValue ) \
{\
    __logging_mtx.lock();\
    std::cout << "[";\
    size_t __percent = 100 * ( __value + 1 ) / __maxValue;\
    for( size_t __progress = 0; __progress < 100; ++__progress )\
        std::cout << ( __progress <= __percent ? "=" : " " );\
    std::cout << "] " << int( __percent ) << " %\r";\
    std::cout.flush();\
    if( __value >= __maxValue - 1 )\
        std::cout << std::endl;\
    __logging_mtx.unlock();\
}

#endif
