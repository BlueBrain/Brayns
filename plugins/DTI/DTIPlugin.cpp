/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns
 * <https://github.com/BlueBrain/Brayns-UC-DTI>
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

#include "DTIPlugin.h"

#include <io/DTILoader.h>

#include <brayns/common/Log.h>
#include <brayns/pluginapi/PluginAPI.h>

namespace dti
{
void DTIPlugin::init()
{
    auto &registry = _api->getLoaderRegistry();
    registry.registerLoader(std::make_unique<DTILoader>());
}
} // namespace dti

extern "C" brayns::ExtensionPlugin *brayns_plugin_create(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    brayns::Log::info("[DTI] Loading DTI plugin.");
    return new dti::DTIPlugin();
}
