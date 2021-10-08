/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#include "BBICPlugin.h"
#include "BBICLoader.h"

#include <brayns/engine/Model.h>
#include <brayns/engine/Scene.h>
#include <brayns/pluginapi/PluginAPI.h>

namespace bbic
{
void Plugin::init()
{
    auto& scene = _api->getScene();
    scene.getLoaderRegistry().registerLoader(
        std::make_unique<Loader>(scene, this));
}

void Plugin::preRender()
{
    for (auto& volumeModel : _volumeModels)
        volumeModel.updateActiveVolume();
}

void Plugin::addModel(VolumeModel&& volumeModel)
{
    volumeModel.triggerRender = [api = _api] { api->triggerRender(); };
    _volumeModels.emplace_back(std::move(volumeModel));
}

void Plugin::removeModel(const size_t modelID)
{
    auto i =
        std::find_if(_volumeModels.begin(), _volumeModels.end(),
                     [modelID](const auto& volumeModel) {
                         return modelID == volumeModel.getModel()->getModelID();
                     });
    if (i != _volumeModels.end())
        _volumeModels.erase(i, _volumeModels.end());
}
} // namespace bbic

extern "C" brayns::ExtensionPlugin* brayns_plugin_create(int, const char**)
{
    return new bbic::Plugin;
}
