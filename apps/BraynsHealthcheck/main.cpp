/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
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
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/WebSocket.h>

struct Settings
{
    std::string uri;
    bool secure = false;
    std::string ca;
};

bool isSecure(std::string_view value)
{
    if (value == "--secure")
    {
        return true;
    }
    throw std::runtime_error("Expected --secure as second argument");
}

Settings parseSettings(int argc, const char **argv)
{
    auto settings = Settings();
    if (argc < 2 || argc > 4)
    {
        throw std::runtime_error("Invalid usage, expected braynsHealthcheck host:port [--secure] [ca]");
    }
    settings.uri = argv[1];
    if (argc > 2)
    {
        settings.secure = isSecure(argv[2]);
    }
    if (settings.secure && argc > 3)
    {
        settings.ca = argv[3];
    }
    return settings;
}

Poco::Net::Context::Ptr createSslContext(const std::string &caLocation)
{
    auto usage = Poco::Net::Context::TLS_CLIENT_USE;
    return Poco::makeAuto<Poco::Net::Context>(usage, caLocation);
}

std::unique_ptr<Poco::Net::HTTPClientSession> createSession(const Settings &settings)
{
    auto address = Poco::Net::SocketAddress(settings.uri);
    if (!settings.secure)
    {
        return std::make_unique<Poco::Net::HTTPClientSession>(address);
    }
    auto host = address.host();
    auto ip = host.toString();
    auto port = address.port();
    auto context = createSslContext(settings.ca);
    return std::make_unique<Poco::Net::HTTPSClientSession>(ip, port, context);
}

void run(const Settings &settings)
{
    auto session = createSession(settings);
    auto request = Poco::Net::HTTPRequest(Poco::Net::HTTPRequest::HTTP_1_1);
    auto response = Poco::Net::HTTPResponse();
    auto websocket = Poco::Net::WebSocket(*session, request, response);
    websocket.close();
}

int main(int argc, const char **argv)
{
    try
    {
        auto settings = parseSettings(argc, argv);
        run(settings);
    }
    catch (const Poco::Exception &e)
    {
        std::cout << "Test connection failed: " << e.displayText() << ".\n";
        return -1;
    }
    catch (const std::exception &e)
    {
        std::cout << "Test connection failed: " << e.what() << ".\n";
        return -1;
    }
    catch (...)
    {
        std::cout << "Test connection failed: Unknown error.";
        return -1;
    }
    return 0;
}
