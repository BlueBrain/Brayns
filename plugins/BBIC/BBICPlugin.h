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

#pragma once

#include <brayns/common/types.h>
#include <brayns/pluginapi/ExtensionPlugin.h>

#include "BBICFile.h"
#include "VolumeModel.h"
#include <list>

namespace bbic
{
class Plugin : public brayns::ExtensionPlugin
{
public:
    void init() final;

    void preRender() final;

    void addModel(VolumeModel&& model);
    void removeModel(const size_t modelID);

private:
    std::list<VolumeModel> _volumeModels;
    brayns::PluginAPI* _api{nullptr};
};
}
