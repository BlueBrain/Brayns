# Copyright (c) 2015-2024 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: adrien.fleury@epfl.ch
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import brayns
from testapi.simple_test_case import SimpleTestCase


class TestInstance(SimpleTestCase):
    def test_context(self) -> None:
        with self.instance as instance:
            instance.poll(block=False)
        self.assertFalse(instance.connected)

    def test_connected(self) -> None:
        self.assertTrue(self.instance.connected)
        self.instance.disconnect()
        self.assertFalse(self.instance.connected)

    def test_disconnect(self) -> None:
        self.instance.disconnect()
        self.assertFalse(self.instance.connected)
        with self.assertRaises(brayns.WebSocketError):
            self.instance.request("test", 123)

    def test_request(self) -> None:
        test = self.instance.request("schema", {"endpoint": "get-version"})
        self.assertIsInstance(test, dict)
        self.assertTrue(test)
        with self.assertRaises(brayns.JsonRpcError):
            self.instance.request("invalid")

    def test_execute(self) -> None:
        reply = self.instance.execute("snapshot", {})
        self.assertIsInstance(reply.result, dict)
        self.assertTrue(reply.result)
        self.assertTrue(reply.binary)

    def test_task(self) -> None:
        task = self.instance.task("snapshot", {})
        progresses = list(task)
        reply = task.wait_for_reply()
        self.assertTrue(progresses)
        self.assertIsInstance(reply.result, dict)
        self.assertTrue(reply.result)
        self.assertTrue(reply.binary)

    def test_is_running(self) -> None:
        request = brayns.JsonRpcRequest(0, "registry")
        task = self.instance.send(request)
        self.assertTrue(self.instance.is_running(0))
        task.wait_for_reply()

    def test_send(self) -> None:
        string = brayns.JsonRpcRequest("test", "registry")
        task = self.instance.send(string)
        task.wait_for_reply()
        integer = brayns.JsonRpcRequest(0, "registry")
        task = self.instance.send(integer)
        task.wait_for_reply()
        notification = brayns.JsonRpcRequest(None, "registry")
        task = self.instance.send(notification)
        task.wait_for_reply()

    def test_poll(self) -> None:
        task = self.instance.task("registry")
        self.instance.poll(block=True)
        self.assertTrue(task.is_ready())
        task.wait_for_reply()

    def test_cancel(self) -> None:
        with self.assertRaises(brayns.JsonRpcError):
            self.instance.cancel(0)
