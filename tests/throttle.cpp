/* Copyright (c) 2018, EPFL/Blue Brain Project
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
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

#define BOOST_TEST_MODULE throttle

#include <boost/test/unit_test.hpp>

#include <brayns/common/Timer.h>

#include "../plugins/RocketsPlugin/Throttle.h"

BOOST_AUTO_TEST_CASE(timeout_not_cleared)
{
    brayns::Timeout timeout;
    std::atomic_bool called{false};
    timeout.set([&called] { called = true; }, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    BOOST_CHECK(called);
}

BOOST_AUTO_TEST_CASE(timeout_with_clear)
{
    brayns::Timer timer;
    timer.start();

    brayns::Timeout timeout;

    std::atomic_bool called{false};
    timeout.set([&called] { called = true; }, 500);
    timeout.clear();
    BOOST_CHECK(!called);

    timer.stop();
    BOOST_CHECK_LE(timer.milliseconds(), 500);

    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    BOOST_CHECK(!called);
}

BOOST_AUTO_TEST_CASE(timeout_set_while_not_cleared)
{
    brayns::Timeout timeout;
    std::atomic_bool called{false};
    timeout.set([&called] { called = true; }, 10000);
    BOOST_CHECK_THROW(timeout.set([&called] { called = true; }, 10000),
                      std::logic_error);
    timeout.clear();
    BOOST_CHECK(!called);
}

BOOST_AUTO_TEST_CASE(timeout_clear_while_already_done)
{
    brayns::Timeout timeout;
    std::atomic_bool called{false};
    timeout.set([&called] { called = true; }, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    BOOST_CHECK(called);
    BOOST_CHECK_NO_THROW(timeout.clear());
}

BOOST_AUTO_TEST_CASE(throttle_spam_limit)
{
    brayns::Throttle throttle;
    std::atomic_size_t numCalls{0};

    brayns::Timer timer;
    timer.start();
    while (true)
    {
        throttle([&] { ++numCalls; }, 10);
        if (timer.elapsed() >= 0.1)
            break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    BOOST_CHECK_MESSAGE(numCalls >= 10 && numCalls <= 12,
                        std::to_string(numCalls));
}

BOOST_AUTO_TEST_CASE(throttle_spam_check_delayed_call)
{
    brayns::Throttle throttle;
    std::atomic_size_t numCalls{0};

    while (numCalls < 2)
        throttle([&] { ++numCalls; }, 5);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    BOOST_CHECK_EQUAL(numCalls, 3);
}

BOOST_AUTO_TEST_CASE(throttle_one)
{
    brayns::Throttle throttle;
    std::atomic_size_t numCalls{0};

    throttle([&] { ++numCalls; }, 1);
    BOOST_CHECK_EQUAL(numCalls, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    BOOST_CHECK_EQUAL(numCalls, 1);
}
