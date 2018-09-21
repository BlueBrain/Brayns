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

#include "ExtensionPluginFactory.h"

#include "ExtensionPlugin.h"

namespace brayns
{
ExtensionPluginFactory::~ExtensionPluginFactory()
{
    clear();
}

void ExtensionPluginFactory::remove(ExtensionPluginPtr plugin)
{
    ExtensionPlugins::iterator it =
        std::find(_plugins.begin(), _plugins.end(), plugin);
    if (it != _plugins.end())
        _plugins.erase(it);
}

void ExtensionPluginFactory::add(ExtensionPluginPtr plugin)
{
    remove(plugin);
    _plugins.push_back(plugin);
}

void ExtensionPluginFactory::clear()
{
    _plugins.clear();
}

void ExtensionPluginFactory::preRender()
{
    for (ExtensionPluginPtr plugin : _plugins)
        plugin->preRender();
}

void ExtensionPluginFactory::postRender()
{
    for (ExtensionPluginPtr plugin : _plugins)
        plugin->postRender();
}
} // namespace brayns
