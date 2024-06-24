/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <brayns/core/Version.h>
#include <brayns/core/cli/CommandLine.h>
#include <brayns/core/utils/Logger.h>

using namespace brayns::experimental;
using brayns::createConsoleLogger;
using brayns::getCopyright;

namespace brayns::experimental
{
struct Settings
{
    bool help = false;
    bool version = false;
    std::string host;
    std::uint16_t port = 0;
    std::size_t maxClient = 0;
    std::size_t maxFrameSize = 0;
    bool sslEnabled = false;
    // SslSettings ssl = {};
};

template<>
struct ArgvSettingsReflector<Settings>
{
    static auto reflect()
    {
        auto builder = ArgvBuilder<Settings>();
        builder.description(getCopyright());
        builder.option("help", [](auto &settings) { return &settings.help; })
            .description("Display this help message")
            .defaultValue(false);
        builder.option("version", [](auto &settings) { return &settings.version; })
            .description("Display brayns copyright with version")
            .defaultValue(false);
        builder.option("host", [](auto &settings) { return &settings.host; })
            .description("Websocket server hostname, use 0.0.0.0 to allow any host to connect")
            .defaultValue("localhost");
        builder.option("port", [](auto &settings) { return &settings.port; })
            .description("Websocket server port")
            .defaultValue(5000);
        builder.option("max-client", [](auto &settings) { return &settings.maxClient; })
            .description("Maximum number of simultaneously connected clients")
            .defaultValue(2);
        builder.option("max-frame-size", [](auto &settings) { return &settings.maxFrameSize; })
            .description("Maximum frame size the websocket server accepts")
            .defaultValue(std::numeric_limits<int>::max());
        builder.option("ssl", [](auto &settings) { return &settings.sslEnabled; })
            .description("Enable SSL for websocket server, requires a certificate and a private key")
            .defaultValue(false);
        /*builder.option("private-key-file", [](auto &settings) { return &settings.ssl.privateKeyFile; })
            .description("SSL private key used by the websocket server")
            .defaultValue("");
        builder.option("certificate-file", [](auto &settings) { return &settings.ssl.certificateFile; })
            .description("SSL certificate the websocket server will provide to clients")
            .defaultValue("");
        builder.option("ca-location", [](auto &settings) { return &settings.ssl.caLocation; })
            .description("Path to a certificate file to use as certification authority or a CA directory")
            .defaultValue("");
        builder.option("private-key-passphrase", [](auto &settings) { return &settings.ssl.privateKeyPassphrase; })
            .description("Passphrase for the private key if encrypted")
            .defaultValue("");*/
        return builder.build();
    }
};
}

int main(int argc, const char **argv)
{
    auto logger = createConsoleLogger("Brayns");

    try
    {
        auto settings = parseArgvAs<Settings>(argc, argv);

        if (settings.version)
        {
            std::cout << getCopyright() << '\n';
            return 0;
        }

        if (settings.help)
        {
            std::cout << getArgvHelp<Settings>() << '\n';
            return 0;
        }
    }
    catch (const std::exception &e)
    {
        logger.fatal("Fatal error: '{}'.", e.what());
        return 1;
    }
    catch (...)
    {
        logger.fatal("Unknown fatal error.");
        return 1;
    }

    return 0;
}
