/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
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

#include <doctest/doctest.h>

#include <brayns/network/jsonrpc/JsonRpcException.h>
#include <brayns/network/task/TaskManager.h>

#include "MockWebSocket.h"

struct TaskSettings
{
    brayns::ClientRef client;
    brayns::RequestId id;
    std::string method;
    bool priority = false;
};

struct TaskController
{
    bool executed = false;
    bool cancelled = false;
    bool disconnected = false;
};

class MockTask : public brayns::ITask
{
public:
    MockTask(TaskSettings settings, TaskController &controller)
        : _settings(std::move(settings))
        , _controller(controller)
    {
    }

    virtual const brayns::ClientRef &getClient() const override
    {
        return _settings.client;
    }

    virtual const brayns::RequestId &getId() const override
    {
        return _settings.id;
    }

    virtual const std::string &getMethod() const override
    {
        return _settings.method;
    }

    virtual bool hasPriority() const override
    {
        return _settings.priority;
    }

    virtual void run() override
    {
        _controller.executed = true;
    }

    virtual void cancel() override
    {
        _controller.cancelled = true;
    }

    virtual void disconnect() override
    {
        _controller.disconnected = true;
    }

private:
    TaskSettings _settings;
    TaskController &_controller;
};

class TaskHelper
{
public:
    static void registerTask(TaskSettings settings, TaskController &controller, brayns::TaskManager &manager)
    {
        auto task = std::make_unique<MockTask>(std::move(settings), controller);
        manager.add(std::move(task));
    }

    static brayns::ClientRef newClient(size_t id)
    {
        auto socket = std::make_shared<MockWebSocket>(id);
        return brayns::ClientRef(std::move(socket));
    }
};

TEST_CASE("TaskManager")
{
    auto client = TaskHelper::newClient(0);
    auto id = brayns::RequestId(0);
    auto method = std::string("test");
    auto otherClient = TaskHelper::newClient(1);
    auto otherId = brayns::RequestId(1);

    SUBCASE("Running tasks normally")
    {
        auto settings = TaskSettings{client, id, method};
        auto manager = brayns::TaskManager();

        auto first = TaskController();
        TaskHelper::registerTask(settings, first, manager);

        auto second = TaskController();
        TaskHelper::registerTask(settings, second, manager);

        CHECK_FALSE(first.executed);
        CHECK_FALSE(second.executed);

        manager.runAllTasks();

        CHECK(first.executed);
        CHECK(second.executed);
    }
    SUBCASE("Running tasks with priority")
    {
        auto settings = TaskSettings{client, id, method};
        auto manager = brayns::TaskManager();

        auto first = TaskController();
        TaskHelper::registerTask(settings, first, manager);

        auto priority = TaskController();
        settings.priority = true;
        TaskHelper::registerTask(settings, priority, manager);

        CHECK_FALSE(first.executed);
        CHECK(priority.executed);

        manager.runAllTasks();

        CHECK(first.executed);
    }
    SUBCASE("Client disconnection")
    {
        auto settings = TaskSettings{client, id, method};
        auto manager = brayns::TaskManager();

        auto match = TaskController();
        TaskHelper::registerTask(settings, match, manager);

        auto differentClient = TaskController();
        settings.client = otherClient;
        TaskHelper::registerTask(settings, differentClient, manager);

        auto differentId = TaskController();
        settings.client = client;
        settings.id = otherId;
        TaskHelper::registerTask(settings, differentId, manager);

        manager.disconnect(client);

        CHECK(match.disconnected);
        CHECK_FALSE(differentClient.disconnected);
        CHECK(differentId.disconnected);

        manager.runAllTasks();

        CHECK(match.executed);
        CHECK(differentClient.executed);
        CHECK(differentId.executed);
    }
    SUBCASE("Cancel task")
    {
        auto settings = TaskSettings{client, id, method};
        auto manager = brayns::TaskManager();

        auto match = TaskController();
        TaskHelper::registerTask(settings, match, manager);

        auto differentClient = TaskController();
        settings.client = otherClient;
        TaskHelper::registerTask(settings, differentClient, manager);

        auto differentId = TaskController();
        settings.client = client;
        settings.id = otherId;
        TaskHelper::registerTask(settings, differentId, manager);

        manager.cancel(client, id);

        CHECK(match.cancelled);
        CHECK_FALSE(differentClient.cancelled);
        CHECK_FALSE(differentId.cancelled);

        manager.runAllTasks();

        CHECK(match.executed);
        CHECK(differentClient.executed);
        CHECK(differentId.executed);
    }
    SUBCASE("Cancel non-existing task")
    {
        auto manager = brayns::TaskManager();
        auto cancel = [&] { manager.cancel(client, id); };
        CHECK_THROWS_AS(cancel(), brayns::InvalidParamsException);
    }
    SUBCASE("Cancel task which has no ID")
    {
        auto emptyId = brayns::RequestId();
        auto settings = TaskSettings{client, emptyId, method};
        auto manager = brayns::TaskManager();

        auto controller = TaskController();
        TaskHelper::registerTask(settings, controller, manager);

        auto cancel = [&] { manager.cancel(client, brayns::RequestId()); };
        CHECK_THROWS_AS(cancel(), brayns::InvalidParamsException);
    }
}
