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

#include "OptiXUtils.h"

#include <brayns/common/log.h>

#if BRAYNS_USE_LUNCHBOX
#  include <lunchbox/file.h>
#endif

namespace brayns
{

const std::string getPTXPath( const std::string& filename )
{
#if BRAYNS_USE_LUNCHBOX
    const std::string basePath = lunchbox::getExecutablePath() +
                                 "/../share/Brayns/ptx/";
#else
    const std::string basePath = "../share/Brayns/ptx/";
#endif
    const std::string fullPath =
        basePath + "braynsOptiXCudaPlugin_generated_" + filename + ".ptx";
    BRAYNS_DEBUG << "[PTX] " << fullPath << std::endl;
    return fullPath;
}

}
