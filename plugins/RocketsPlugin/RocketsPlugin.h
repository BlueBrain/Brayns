/* Copyright (c) 2015-2018 EPFL/Blue Brain Project
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

#ifndef ROCKETSPLUGIN_H
#define ROCKETSPLUGIN_H

#include <brayns/api.h>
#include <brayns/common/ActionInterface.h>
#include <brayns/common/types.h>
#include <brayns/pluginapi/ExtensionPlugin.h>

namespace brayns
{
/**
   The RocketsPlugin is in charge of exposing a both an http/REST interface to
   the outside world. The http server is configured according
   to the --http-server parameter provided by ApplicationParameters.
 */
class RocketsPlugin : public ExtensionPlugin, public ActionInterface
{
public:
    RocketsPlugin(EnginePtr engine, PluginAPI* api);

    /**
     * In case no event loop is available, this processes in- and outgoing HTTP
     * and websocket messages.
     *
     * Otherwise, this is a NOP as the incoming message processing is done by
     * the SocketListener.
     */
    BRAYNS_API void preRender() final;

    /**
     * Enqueue modified and registered objects for broadcast that have changed
     * after the rendering is finished (framebuffer).
     */
    BRAYNS_API void postRender() final;

private:
    class Impl;
    std::shared_ptr<Impl> _impl;

    void registerNotification(
        const RpcParameterDescription& desc, const PropertyMap& input,
        const std::function<void(PropertyMap)>& action) final;

    void registerNotification(const RpcDescription& desc,
                              const std::function<void()>& action) final;
    void registerRequest(
        const RpcParameterDescription& desc, const PropertyMap& input,
        const PropertyMap& output,
        const std::function<PropertyMap(PropertyMap)>& action) final;

    void registerRequest(const RpcDescription& desc, const PropertyMap& output,
                         const std::function<PropertyMap()>& action) final;

    void _registerRequest(const std::string& name,
                          const RetParamFunc& action) final;
    void _registerRequest(const std::string& name, const RetFunc& action) final;
    void _registerNotification(const std::string& name,
                               const ParamFunc& action) final;
    void _registerNotification(const std::string& name,
                               const VoidFunc& action) final;
};
} // namespace brayns

#endif
