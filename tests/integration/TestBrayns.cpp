/* Copyright (c) 2015-2023, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Authors: Daniel.Nachbaur@epfl.ch
 *                      Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <brayns/Brayns.h>

#include <doctest/doctest.h>

#include <Poco/Net/ServerSocket.h>

namespace
{
class BraynsLauncher
{
public:
    static void launchValid(std::vector<const char *> args)
    {
        CHECK_NOTHROW(_launch(args));
    }

    static void launchInvalid(std::vector<const char *> args, std::string_view message)
    {
        auto messagePtr = message.data();
        auto messageSize = static_cast<doctest::String::size_type>(message.length());
        CHECK_THROWS_WITH(_launch(args), doctest::Contains(doctest::String(messagePtr, messageSize)));
    }

private:
    static void _launch(std::vector<const char *> args)
    {
        auto argc = static_cast<int>(args.size());
        auto argv = args.data();
        auto brayns = brayns::Brayns(argc, argv);
        (void)brayns;
    }
};

class Server
{
public:
    Server(const std::string &uri)
    {
        _thread = std::thread(
            [&]
            {
                auto address = Poco::Net::SocketAddress(uri);
                auto server = Poco::Net::ServerSocket(address);
                while (_running)
                {
                    auto client = server.acceptConnection();
                    (void)client;
                }
            });
    }

    ~Server()
    {
        close();
    }

    void close()
    {
        if (!std::exchange(_running, false))
        {
            return;
        }

        try
        {
            _thread.join();
        }
        catch (...)
        {
        }
    }

private:
    std::thread _thread;
    bool _running = true;
};
}

TEST_CASE("Construction")
{
    SUBCASE("Valid constructions")
    {
        BraynsLauncher::launchValid({"brayns"});
        BraynsLauncher::launchValid(
            {"brayns",
             "--uri",
             "0.0.0.0:5000",
             "--max-clients",
             "5",
             "--log-level",
             "debug",
             "--window-size",
             "100 100"});
    }
    SUBCASE("Client mode")
    {
        auto server = Server("0.0.0.0:5000");

        BraynsLauncher::launchValid(
            {"brayns",
             "--uri",
             "0.0.0.0:5000",
             "--client",
             "true",
             "--log-level",
             "debug",
             "--window-size",
             "100 100"});

        server.close();
    }
}
