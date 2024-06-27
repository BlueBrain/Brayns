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

#include "Launcher.h"

#include <brayns/core/service/Service.h>
#include <brayns/core/utils/Logger.h>

namespace brayns
{
void runService(const ServiceSettings &settings)
{
    auto level = getEnumValue<LogLevel>(settings.logLevel);

    auto logger = createConsoleLogger("brayns");
    logger.setLevel(level);

    auto ssl = std::optional<SslSettings>();

    if (settings.ssl)
    {
        ssl = SslSettings{
            .privateKeyFile = settings.privateKeyFile,
            .certificateFile = settings.certificateFile,
            .caLocation = settings.caLocation,
            .privateKeyPassphrase = settings.privateKeyPassphrase,
        };
    }

    auto server = WebSocketServerSettings{
        .host = settings.host,
        .port = settings.port,
        .maxThreadCount = settings.maxThreadCount,
        .maxQueueSize = settings.maxQueueSize,
        .maxFrameSize = settings.maxFrameSize,
        .ssl = std::move(ssl),
    };

    auto endpoints = EndpointRegistry({});

    auto tasks = TaskManager();

    auto context = std::make_unique<ServiceContext>(ServiceContext{
        .logger = std::move(logger),
        .server = std::move(server),
        .endpoints = std::move(endpoints),
        .tasks = std::move(tasks),
    });

    auto service = Service(std::move(context));

    service.run();
}
}
