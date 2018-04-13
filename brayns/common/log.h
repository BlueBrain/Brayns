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
#include <thread>

#define BRAYNS_ERROR std::cerr << "[ERROR] "
#define BRAYNS_WARN std::cerr << "[WARN ] "
#define BRAYNS_INFO std::cout << "[INFO ] "
#ifdef NDEBUG
#define BRAYNS_DEBUG \
    if (false)       \
    std::cout
#define BRAYNS_FCT_ENTRY
#else
#define BRAYNS_DEBUG std::cout << "[DEBUG] "
#define BRAYNS_FCT_ENTRY                                                      \
    std::cout << "[" << std::this_thread::get_id() << "][ENTRY] " << __FILE__ \
              << ": " << __FUNCTION__ << std::endl;
#endif

#define BRAYNS_THROW(exc)                        \
    {                                            \
        BRAYNS_ERROR << exc.what() << std::endl; \
        throw exc;                               \
    }

#endif
