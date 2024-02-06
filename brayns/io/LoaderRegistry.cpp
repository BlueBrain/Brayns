/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel.Nachbaur <daniel.nachbaur@epfl.ch>
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

#include "LoaderRegistry.h"

#include <filesystem>

#include <brayns/io/loaders/mesh/MeshLoader.h>
#include <brayns/io/loaders/volume/VolumeLoader.h>
#include <brayns/utils/Log.h>

namespace brayns
{
LoaderRef::LoaderRef(LoaderInfo info, std::unique_ptr<ILoader> loader):
    _info(std::move(info)),
    _loader(std::move(loader))
{
}

const LoaderInfo &LoaderRef::getInfo() const
{
    return _info;
}

const std::string &LoaderRef::getPlugin() const
{
    return _info.plugin;
}

const std::string &LoaderRef::getName() const
{
    return _info.name;
}

const std::vector<std::string> &LoaderRef::getExtensions() const
{
    return _info.extensions;
}

bool LoaderRef::canLoadBinary() const
{
    return _info.binary;
}

const JsonSchema &LoaderRef::getSchema() const
{
    return _info.schema;
}

std::vector<std::shared_ptr<Model>> LoaderRef::loadBinary(const RawBinaryLoaderRequest &request)
{
    return _loader->loadBinary(request);
}

std::vector<std::shared_ptr<Model>> LoaderRef::loadFile(const RawFileLoaderRequest &request)
{
    return _loader->loadFile(request);
}

LoaderRef LoaderBuilder::build(std::string plugin, std::unique_ptr<ILoader> loader)
{
    auto info = LoaderInfo();
    info.plugin = std::move(plugin);
    info.name = loader->getName();
    info.extensions = loader->getExtensions();
    info.binary = loader->canLoadBinary();
    info.schema = loader->getSchema();
    return LoaderRef(std::move(info), std::move(loader));
}

std::vector<LoaderInfo> LoaderRegistry::getInfos() const
{
    auto loaders = std::vector<LoaderInfo>();
    loaders.reserve(_loaders.size());
    for (const auto &loader : _loaders)
    {
        loaders.push_back(loader.getInfo());
    }
    return loaders;
}

LoaderRef *LoaderRegistry::findByName(const std::string &name)
{
    for (auto &loader : _loaders)
    {
        if (loader.getName() == name)
        {
            return &loader;
        }
    }
    return nullptr;
}

LoaderRef *LoaderRegistry::findByFormat(const std::string &format)
{
    for (auto &loader : _loaders)
    {
        for (const auto &extension : loader.getExtensions())
        {
            if (extension == format)
            {
                return &loader;
            }
        }
    }
    return nullptr;
}

void LoaderRegistry::add(std::string plugin, std::unique_ptr<ILoader> loader)
{
    auto ref = LoaderBuilder::build(std::move(plugin), std::move(loader));
    Log::info("Plugin '{}' registered loader '{}'.", ref.getPlugin(), ref.getName());
    _loaders.push_back(std::move(ref));
}

LoaderRegistryBuilder::LoaderRegistryBuilder(std::string plugin, LoaderRegistry &loaders):
    _plugin(std::move(plugin)),
    _loaders(loaders)
{
}

void LoaderRegistryBuilder::addLoader(std::unique_ptr<ILoader> loader)
{
    _loaders.add(_plugin, std::move(loader));
}

LoaderRegistry CoreLoaderRegistry::create()
{
    auto loaders = LoaderRegistry();
    auto builder = LoaderRegistryBuilder("Core", loaders);
    builder.add<MeshLoader>();
    builder.add<MHDVolumeLoader>();
    builder.add<RawVolumeLoader>();
    return loaders;
}
} // namespace brayns
