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

#include "../plugins/Rockets/Throttle.h"

struct TestTimer
{
    void start() { timer.start(); }
    void onCalled()
    {
        std::unique_lock<std::mutex> lock(mutex);
        timer.stop();
        called = true;
        condition.notify_one();
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex);
        while (!called)
            condition.wait(lock);
    }

    void wait_for(const int64_t waitTime)
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait_for(lock, std::chrono::milliseconds(waitTime),
                           [&] { return !called; });
    }

    auto milliseconds() const { return timer.milliseconds(); }
    auto wasCalled() const { return called; }
private:
    bool called = false;
    std::mutex mutex;
    std::condition_variable condition;
    brayns::Timer timer;
};

BOOST_AUTO_TEST_CASE(timeout_not_cleared)
{
    brayns::Timeout timeout;

    const int64_t waitTime = 5;
    TestTimer timer;
    timer.start();
    timeout.set([&] { timer.onCalled(); }, waitTime);
    timer.wait();

    BOOST_CHECK_GE(timer.milliseconds(), waitTime);
    BOOST_CHECK(timer.wasCalled());
}

BOOST_AUTO_TEST_CASE(timeout_with_clear)
{
    brayns::Timeout timeout;

    const int64_t waitTime = 10;
    TestTimer timer;
    timer.start();
    timeout.set([&] { timer.onCalled(); }, waitTime);
    timeout.clear();
    timer.wait_for(waitTime);

    BOOST_CHECK(!timer.wasCalled());
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
    TestTimer timer;
    timeout.set([&] { timer.onCalled(); }, 1);
    timer.wait();
    BOOST_CHECK(timer.wasCalled());
    BOOST_CHECK_NO_THROW(timeout.clear());
}

BOOST_AUTO_TEST_CASE(throttle_spam_limit)
{
    brayns::Throttle throttle;
    size_t numCalls{0};
    std::atomic_bool done{false};

    const int64_t waitTime = 10;
    brayns::Timer timer;
    timer.start();
    while (!done)
    {
        throttle(
            [&] {
                ++numCalls;
                if (timer.elapsed() >= 0.1)
                    done = true;
            },
            waitTime);
    }

    // throttle time of 10ms within a 100ms time window should yield 10 calls
    BOOST_CHECK_MESSAGE(numCalls >= 10 && numCalls <= 12,
                        std::to_string(numCalls));
}

BOOST_AUTO_TEST_CASE(throttle_spam_check_delayed_call)
{
    brayns::Throttle throttle;
    size_t numCalls{0};
    std::atomic_bool done{false};
    TestTimer timer;

    while (!done)
        throttle(
            [&] {
                if (++numCalls > 2)
                {
                    done = true;
                    timer.onCalled();
                }
            },
            5);
    timer.wait();
    BOOST_CHECK_EQUAL(numCalls, 3);
}

BOOST_AUTO_TEST_CASE(throttle_one)
{
    brayns::Throttle throttle;
    size_t numCalls{0};

    throttle([&] { ++numCalls; }, 1);
    BOOST_CHECK_EQUAL(numCalls, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    BOOST_CHECK_EQUAL(numCalls, 1);
}
