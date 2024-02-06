/* Copyright (c) 2015-2024 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 *
 * Responsible Author: adrien.fleury@epfl.ch
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

#include "MoleculeExplorerPlugin.h"

#include <brayns/utils/Log.h>

#include <memory>

#include "io/protein/ProteinLoader.h"
#include "io/xyz/XyzLoader.h"

MoleculeExplorerPlugin::MoleculeExplorerPlugin(brayns::PluginAPI &api)
{
    auto &registry = api.getLoaderRegistry();
    auto loaders = brayns::LoaderRegistryBuilder("Molecule Explorer", registry);
    loaders.add<ProteinLoader>();
    loaders.add<XyzLoader>();
}

extern "C" std::unique_ptr<brayns::IPlugin> brayns_create_plugin(brayns::PluginAPI &api)
{
    brayns::Log::info("[MoleculeExplorer] Loading Molecule Explorer plugin.");
    return std::make_unique<MoleculeExplorerPlugin>(api);
}
