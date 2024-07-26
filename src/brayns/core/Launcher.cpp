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

#include <iostream>

#include <brayns/core/endpoints/CoreEndpoints.h>
#include <brayns/core/endpoints/ObjectEndpoints.h>
#include <brayns/core/service/Service.h>
#include <brayns/core/utils/Logger.h>
#include <brayns/core/utils/String.h>

namespace
{
using namespace brayns;

WebSocketServerSettings extractServerSettings(const ServiceSettings &settings)
{
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

    return {
        .host = settings.host,
        .port = settings.port,
        .maxThreadCount = settings.maxThreadCount,
        .maxQueueSize = settings.maxQueueSize,
        .maxFrameSize = settings.maxFrameSize,
        .ssl = std::move(ssl),
    };
}

void startServerAndRunService(const ServiceSettings &settings, Logger &logger)
{
    auto level = getEnumValue<LogLevel>(settings.logLevel);
    logger.setLevel(level);

    logger.info("{}", getCopyright());

    logger.debug("Service options:{}", stringifyArgvSettings(settings));

    auto token = StopToken();

    logger.info("Building JSON-RPC API");

    auto api = Api();

    auto builder = ApiBuilder();

    addCoreEndpoints(builder, api, token);

    auto objects = ObjectManager();

    addObjectEndpoints(builder, objects);

    api = builder.build();

    logger.info("JSON-RCP API ready");

    auto methods = api.getMethods();
    logger.debug("Available methods:\n    {}", join(methods, "\n    "));

    logger.info("Starting websocket server on {}:{}", settings.host, settings.port);

    auto serverSettings = extractServerSettings(settings);
    auto server = startServer(serverSettings, logger);

    logger.info("Websocket server started");

    logger.info("Service running");
    runService(server, api, token, logger);
}
}

namespace brayns
{
int runServiceFromArgv(int argc, const char **argv)
{
    auto logger = createConsoleLogger("brayns");

    try
    {
        auto settings = parseArgvAs<ServiceSettings>(argc, argv);

        if (settings.help)
        {
            std::cout << getArgvHelp<ServiceSettings>() << '\n';
            return 0;
        }

        if (settings.version)
        {
            std::cout << getCopyright() << '\n';
            return 0;
        }

        startServerAndRunService(settings, logger);

        return 0;
    }
    catch (const std::exception &e)
    {
        logger.fatal("Fatal error: {}", e.what());
        return 1;
    }
    catch (...)
    {
        logger.fatal("Unknown fatal error");
        return 1;
    }
}
}
