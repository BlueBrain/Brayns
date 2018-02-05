/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
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

#include "EngineFactory.h"

#include <brayns/common/engine/Engine.h>
#include <brayns/common/log.h>

#if (BRAYNS_USE_OSPRAY)
#include <plugins/engines/ospray/OSPRayEngine.h>
#endif

namespace brayns
{
EngineFactory::EngineFactory(const int argc, const char** argv,
                             ParametersManager& parametersManager)
    : _argc{argc}
    , _argv{argv}
    , _parametersManager{parametersManager}
{
}

std::unique_ptr<Engine> EngineFactory::create(
    const EngineType name BRAYNS_UNUSED)
{
    try
    {
#if (BRAYNS_USE_OSPRAY)
        if (name == EngineType::ospray || name == EngineType::optix)
            return std::make_unique<OSPRayEngine>(_argc, _argv,
                                                  _parametersManager);
#endif
    }
    catch (const std::runtime_error& e)
    {
        BRAYNS_ERROR << "Engine creation failed: " << e.what() << std::endl;
    }
    return std::unique_ptr<Engine>();
}
}
