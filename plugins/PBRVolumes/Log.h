/* Copyright (c) 2020, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
 *
 * This file is part Brayns
 * <https://github.com/BLueBrain/Brayns>
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

#pragma once

#include <ctime>
#include <iostream>

#define PLUGIN_ERROR std::cerr << "[ERROR] [PBRVOLUMES] "
#define PLUGIN_WARN std::cerr << "[WARN ] [PBRVOLUMES] "
#define PLUGIN_INFO std::cout << "[INFO ] [PBRVOLUMES] "
#define PLUGIN_TIMER(__time, __msg)                                  \
    std::cout << "[TIMER] [PBRVOLUMES] [" << __time << "] " << __msg \
              << std::endl;
#ifdef NDEBUG
#define PLUGIN_DEBUG \
    if (false)       \
    std::cout
#else
#define PLUGIN_DEBUG std::cout << "[DEBUG] [PBRVOLUMES] "
#endif

#define PLUGIN_THROW(__message)                 \
    {                                           \
        PLUGIN_ERROR << __message << std::endl; \
        throw std::runtime_error(__message);    \
    }

