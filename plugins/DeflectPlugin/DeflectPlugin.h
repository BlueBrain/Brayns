/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#ifndef DEFLECTPLUGIN_H
#define DEFLECTPLUGIN_H

#include <brayns/api.h>
#include <brayns/common/types.h>
#include <brayns/pluginapi/ExtensionPlugin.h>

namespace brayns
{
class DeflectPlugin : public ExtensionPlugin
{
public:
    DeflectPlugin(EnginePtr engine, PluginAPI* api);

    /** Handle stream setup and incoming events. */
    BRAYNS_API void preRender() final;

    /** Send rendered frame. */
    BRAYNS_API void postRender(FrameBuffer& frameBuffer) final;

private:
    class Impl;
    std::shared_ptr<Impl> _impl;
};
} // namespace brayns

#endif
