/* Copyright (c) 2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
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

#include "BBICLoader.h"
#include "BBICFile.h"
#include "BBICPlugin.h"

#include <brayns/engineapi/Model.h>
#include <brayns/engineapi/Scene.h>

#include <brayns/engineapi/BrickedVolume.h>
#include <brayns/engineapi/Camera.h>
#include <brayns/pluginapi/Plugin.h>

namespace bbic
{
Loader::Loader(brayns::Scene& scene, Plugin* plugin)
    : brayns::Loader(scene)
    , _plugin(plugin)
{
}

std::vector<std::string> Loader::getSupportedExtensions() const
{
    return {"h5"};
}

std::string Loader::getName() const
{
    return "BBICLoader";
}

bool Loader::isSupported(const std::string&, const std::string& extension) const
{
    return extension == "h5";
}

brayns::ModelDescriptorPtr Loader::importFromFile(
    const std::string& fileName, const brayns::LoaderProgress& callback,
    const brayns::PropertyMap& /*properties*/) const
{
    VolumeModel volumeModel(fileName, _scene.createModel(), callback);
    auto modelDesc = volumeModel.getModel();
    _plugin->addModel(std::move(volumeModel));

    modelDesc->onRemoved([plugin = _plugin](const auto& modelDesc_) {
        plugin->removeModel(modelDesc_.getModelID());
    });

    return modelDesc;
}
}
