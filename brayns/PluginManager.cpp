/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Juan Hernando <juan.hernando@epfl.ch>
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

#include "PluginManager.h"

#include <brayns/common/log.h>
#include <brayns/parameters/ParametersManager.h>

#include <brayns/pluginapi/ExtensionPlugin.h>
#include <brayns/pluginapi/PluginAPI.h>
#ifdef BRAYNS_USE_NETWORKING
#include <plugins/Rockets/RocketsPlugin.h>
#endif
#ifdef BRAYNS_USE_DEFLECT
#include <plugins/Deflect/DeflectPlugin.h>
#endif
#ifdef BRAYNS_USE_CIRCUITVIEWER
#include <plugins/CircuitViewer/CircuitViewer.h>
#endif

#include <boost/algorithm/string.hpp>

namespace brayns
{
typedef ExtensionPlugin* (*CreateFuncType)(int, const char**);

PluginManager::PluginManager(int argc, const char** argv)
{
    for (int i = 0; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--plugin") != 0)
            continue;
        if (++i == argc || argv[i][0] == '\0' || argv[i][0] == '-')
        {
            // Do not print anything here, errors will be reported later
            // during option parsing
            continue;
        }

        std::string str(argv[i]);
        boost::trim(str);
        std::vector<std::string> words;
        boost::split(words, str, boost::is_any_of(" "),
                     boost::token_compress_on);

        const char* name = words.front().c_str();
        std::vector<const char*> args;
        for (const auto& w : words)
            args.push_back(w.c_str());

        _loadPlugin(name, args.size(), args.data());
    }

#ifdef BRAYNS_USE_CIRCUITVIEWER
    _extensions.push_back(std::make_shared<CircuitViewer>());
#endif
}

void PluginManager::initPlugins(PluginAPI* api)
{
    // Rockets and Deflect plugins cannot be initialized until we have
    // the command line parameters
    auto& parameters = api->getParametersManager();
    auto& appParameters = parameters.getApplicationParameters();
    auto& streamParameters = parameters.getStreamParameters();

    const bool haveHttpServerURI = !appParameters.getHttpServerURI().empty();

    const bool haveDeflectHost =
        getenv("DEFLECT_HOST") || !streamParameters.getHostname().empty();
    if (haveDeflectHost)
#ifdef BRAYNS_USE_DEFLECT
        _extensions.push_back(std::make_shared<DeflectPlugin>());
#else
        throw std::runtime_error(
            "BRAYNS_DEFLECT_ENABLED was not set, but Deflect host was "
            "specified");
#endif

    if (haveHttpServerURI)
#ifdef BRAYNS_USE_NETWORKING
        // Since the Rockets plugin provides the ActionInterface, it must be
        // initialized before anything else
        _extensions.insert(_extensions.begin(),
                           std::make_shared<RocketsPlugin>());
#else
        throw std::runtime_error(
            "BRAYNS_NETWORKING_ENABLED was not set, but HTTP server URI "
            "was specified");
#endif

    for (const auto& extension : _extensions)
        extension->init(api);
}

void PluginManager::preRender()
{
    for (const auto& extension : _extensions)
        extension->preRender();
}

void PluginManager::postRender()
{
    for (const auto& extension : _extensions)
        extension->postRender();
}

void PluginManager::_loadPlugin(const char* name, int argc, const char* argv[])
{
    try
    {
        DynamicLib library(name);
        auto createSym = library.getSymbolAddress("brayns_plugin_create");
        if (!createSym)
        {
            throw std::runtime_error(
                std::string("Plugin '") + name +
                "' is not a valid Brayns plugin; missing " +
                "brayns_plugin_create()");
        }

        CreateFuncType createFunc = (CreateFuncType)createSym;

        _extensions.emplace_back(createFunc(argc, argv));
        _libs.push_back(std::move(library));
        BRAYNS_INFO << "Loaded plugin '" << name << "'" << std::endl;
    }
    catch (const std::runtime_error& exc)
    {
        BRAYNS_ERROR << exc.what() << std::endl;
    }
}
}
