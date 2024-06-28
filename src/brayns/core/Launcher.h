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

#pragma once

#include <brayns/core/Version.h>
#include <brayns/core/cli/CommandLine.h>

namespace brayns
{
struct ServiceSettings
{
    bool help;
    bool version;
    std::string logLevel;
    std::string host;
    std::uint16_t port;
    std::size_t maxThreadCount;
    std::size_t maxQueueSize;
    std::size_t maxFrameSize;
    bool ssl;
    std::string privateKeyFile;
    std::string certificateFile;
    std::string caLocation;
    std::string privateKeyPassphrase;
};

template<>
struct ArgvSettingsReflector<ServiceSettings>
{
    static auto reflect()
    {
        auto builder = ArgvBuilder<ServiceSettings>();

        builder.description(getCopyright());

        builder.option("help", [](auto &settings) { return &settings.help; })
            .description("Display this help message and exit")
            .defaultValue(false);

        builder.option("version", [](auto &settings) { return &settings.version; })
            .description("Display brayns copyright with version and exit")
            .defaultValue(false);

        builder.option("log-level", [](auto &settings) { return &settings.logLevel; })
            .description("Log level among [trace, debug, info, warn, error, fatal, off]")
            .defaultValue("info");

        builder.option("host", [](auto &settings) { return &settings.host; })
            .description("Websocket server hostname, use 0.0.0.0 to allow any host to connect")
            .defaultValue("localhost");
        builder.option("port", [](auto &settings) { return &settings.port; })
            .description("Websocket server port")
            .defaultValue(5000);
        builder.option("max-thread-count", [](auto &settings) { return &settings.maxThreadCount; })
            .description("Maximum number of threads for the websocket server")
            .defaultValue(2);
        builder.option("max-queue-size", [](auto &settings) { return &settings.maxQueueSize; })
            .description("Maximum number of queued connections before they are rejected")
            .defaultValue(64);
        builder.option("max-frame-size", [](auto &settings) { return &settings.maxFrameSize; })
            .description("Maximum frame size the websocket server accepts")
            .defaultValue(std::numeric_limits<int>::max());

        builder.option("ssl", [](auto &settings) { return &settings.ssl; })
            .description("Enable SSL for websocket server, requires a certificate and a private key")
            .defaultValue(false);

        builder.option("private-key-file", [](auto &settings) { return &settings.privateKeyFile; })
            .description("SSL private key used by the websocket server")
            .defaultValue("");
        builder.option("certificate-file", [](auto &settings) { return &settings.certificateFile; })
            .description("SSL certificate the websocket server will provide to clients")
            .defaultValue("");
        builder.option("ca-location", [](auto &settings) { return &settings.caLocation; })
            .description("Path to an additional certification authority (file or directory)")
            .defaultValue("");
        builder.option("private-key-passphrase", [](auto &settings) { return &settings.privateKeyPassphrase; })
            .description("Passphrase for the private key if encrypted")
            .defaultValue("");

        return builder.build();
    }
};

int runServiceFromArgv(int argc, const char **argv);
}
