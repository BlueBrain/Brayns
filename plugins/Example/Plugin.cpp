/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include "ExamplePlugin.h"

namespace brayns
{
ExamplePlugin::ExamplePlugin(int argc, const char** argv)
{
    std::cout << "Got args: ";
    for (int i = 0; i < argc; ++i)
        std::cout << argv[i] << "; ";
    std::cout << std::endl;
}

void ExamplePlugin::init(PluginAPI* api)
{
    std::cout << "Plugin init" << std::endl;
}

void ExamplePlugin::preRender()
{
    std::cout << "Plugin pre render" << std::endl;
}

void ExamplePlugin::postRender()
{
    std::cout << "Plugin post render" << std::endl;
}

void ExamplePlugin::postSceneLoading()
{
    std::cout << "Plugin post scene loading" << std::endl;
}
}

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int argc,
                                                         const char** argv)
{
    return new brayns::ExamplePlugin(argc, argv);
}
