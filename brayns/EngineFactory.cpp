/* Copyright 2015-2024 Blue Brain Project/EPFL
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

#include <brayns/common/Log.h>
#include <brayns/engine/Engine.h>
#include <brayns/parameters/ParametersManager.h>

namespace brayns
{
typedef Engine* (*CreateFuncType)(int, const char**, ParametersManager&);

EngineFactory::EngineFactory(const int argc, const char** argv,
                             ParametersManager& parametersManager)
    : _argc{argc}
    , _argv{argv}
    , _parametersManager{parametersManager}
{
}

Engine* EngineFactory::create(const std::string& name)
{
    if (_engines.count(name) == 0)
        return _loadEngine(name.c_str(), _argc, _argv);
    return _engines[name].get();
}

Engine* EngineFactory::_loadEngine(const std::string& name, int argc,
                                   const char* argv[])
{
    try
    {
        DynamicLib library(name);
        auto createSym = library.getSymbolAddress("brayns_engine_create");
        if (!createSym)
        {
            throw std::runtime_error(
                std::string("Plugin '") + name +
                "' is not a valid Brayns engine; missing " +
                "brayns_engine_create()");
        }

        CreateFuncType createFunc = (CreateFuncType)createSym;
        if (auto plugin = createFunc(argc, argv, _parametersManager))
        {
            _engines.emplace(name, std::unique_ptr<Engine>(plugin));
            _libs.push_back(std::move(library));
            Log::info("Loaded engine '{}'.", name);
            return plugin;
        }
    }
    catch (const std::runtime_error& exc)
    {
        Log::error("Failed to load engine '{}': {}", name, exc.what());
    }
    return nullptr;
}
} // namespace brayns
