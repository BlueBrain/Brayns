/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include <doctest.h>

#include <brayns/core/parameters/ParametersManager.h>

TEST_CASE("Parameters")
{
    SUBCASE("Simulation parameters")
    {
        auto simulation = brayns::SimulationParameters();

        CHECK(simulation.isModified());
        simulation.resetModified();
        CHECK(!simulation.isModified());

        simulation.setEndFrame(150);
        CHECK(simulation.isModified());
        CHECK(simulation.getEndFrame() == 150);

        simulation.resetModified();
        simulation.setDt(0.01);
        CHECK(simulation.getDt() == doctest::Approx(0.01));
        CHECK(simulation.isModified());

        simulation.resetModified();
        simulation.setFrame(50);
        CHECK(simulation.getFrame() == 50);
        CHECK(simulation.isModified());

        simulation.resetModified();
        simulation.setStartFrame(40);
        CHECK(simulation.getStartFrame() == 40);
        CHECK(simulation.isModified());

        simulation.resetModified();
        CHECK(!simulation.isModified());
        simulation.reset();
        CHECK(simulation.isModified());
        CHECK(simulation.getDt() == 0.0);
        CHECK(simulation.getStartFrame() == 0);
        CHECK(simulation.getEndFrame() == 0);
        CHECK(simulation.getFrame() == 0);
    }
    SUBCASE("Application parameters")
    {
        auto cmd = std::vector<const char *>{"executable", "--window-size", "500, 500", "--log-level", "debug"};
        auto argc = static_cast<int>(cmd.size());
        auto argv = cmd.data();
        auto parameters = brayns::ParametersManager(argc, argv);
        auto &application = parameters.getApplicationParameters();

        CHECK(application.getWindowSize() == brayns::Vector2ui(500));
        CHECK(application.getLogLevel() == brayns::LogLevel::Debug);
    }
    SUBCASE("Network parameters")
    {
        auto cmd = std::vector<const char *>{
            "executable",
            "--secure",
            "true",
            "--max-clients",
            "123",
            "--uri",
            "0.0.0.0:7777",
            "--reconnection-period",
            "456",
            "--private-key-file",
            "/A/B/Key",
            "--private-key-passphrase",
            "789abc",
            "--certificate-file",
            "/A/B/Certificate.ca",
            "--ca-location",
            "/A/B/CertificationAuthorities"};
        auto argc = static_cast<int>(cmd.size());
        auto argv = cmd.data();
        auto parameters = brayns::ParametersManager(argc, argv);

        auto &network = parameters.getNetworkParameters();
        CHECK(network.isSecure());
        CHECK(network.getMaxClients() == 123);
        CHECK(network.getUri() == "0.0.0.0:7777");
        CHECK(network.getReconnectionPeriod() == std::chrono::milliseconds(456));
        CHECK(network.getPrivateKeyFile() == "/A/B/Key");
        CHECK(network.getPrivateKeyPassphrase() == "789abc");
        CHECK(network.getCertificateFile() == "/A/B/Certificate.ca");
        CHECK(network.getCALocation() == "/A/B/CertificationAuthorities");
    }
}
