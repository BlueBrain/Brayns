/* Copyright (c) 2015-2024, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
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

#include <memory>
#include <string>

#include <doctest/doctest.h>

#include <brayns/core/network/common/CancellationToken.h>
#include <brayns/core/network/common/ProgressHandler.h>

#include "MockInterface.h"

class MockRequest
{
public:
    MockRequest(std::string &operation, double &progress):
        _operation(operation),
        _progress(progress)
    {
    }

    void progress(const std::string &operation, double progress) const
    {
        _operation = operation;
        _progress = progress;
    }

private:
    std::string &_operation;
    double &_progress;
};

TEST_CASE("CancellationToken")
{
    SUBCASE("Poll")
    {
        auto interface = MockInterface();
        auto token = brayns::CancellationToken(interface);
        token.poll();
        CHECK(interface.isPolled());
    }
    SUBCASE("Cancel")
    {
        auto interface = MockInterface();
        auto token = brayns::CancellationToken(interface);
        token.cancel();
        CHECK_THROWS_AS(token.poll(), brayns::TaskCancelledException);
        CHECK(interface.isPolled());
    }
    SUBCASE("Reset")
    {
        auto interface = MockInterface();
        auto token = brayns::CancellationToken(interface);
        token.cancel();
        token.reset();
        token.poll();
        CHECK(interface.isPolled());
    }
}

TEST_CASE("ProgressHandler")
{
    SUBCASE("Token reset by construction")
    {
        auto operation = std::string();
        auto amount = 0.0;
        auto request = MockRequest(operation, amount);
        auto interface = MockInterface();
        auto token = brayns::CancellationToken(interface);
        token.cancel();
        brayns::ProgressHandler(token, request);
        token.poll();
    }
    SUBCASE("Notify")
    {
        auto operation = std::string();
        auto amount = 0.0;
        auto request = MockRequest(operation, amount);
        auto interface = MockInterface();
        auto token = brayns::CancellationToken(interface);
        auto progress = brayns::ProgressHandler(token, request);
        progress.notify("test", 0.5);
        CHECK(interface.isPolled());
        CHECK_EQ(operation, "test");
        CHECK_EQ(amount, 0.5);
    }
    SUBCASE("Poll")
    {
        auto operation = std::string();
        auto amount = 0.0;
        auto request = MockRequest(operation, amount);
        auto interface = MockInterface();
        auto token = brayns::CancellationToken(interface);
        auto progress = brayns::ProgressHandler(token, request);
        progress.poll();
        CHECK(interface.isPolled());
    }
}
