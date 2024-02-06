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

import logging
import sys
import unittest

import brayns
from brayns.network.jsonrpc import JsonRpcHandler, JsonRpcTasks


class TestJsonRpcHandler(unittest.TestCase):
    def setUp(self) -> None:
        self._logger = logging.Logger("Test", logging.CRITICAL)
        self._logger.addHandler(logging.StreamHandler(sys.stdout))
        self._tasks = JsonRpcTasks()
        self._handler = JsonRpcHandler(self._tasks, self._logger)

    def test_on_reply(self) -> None:
        reply = brayns.JsonRpcReply(0, 123)
        task = self._tasks.create_task(0)
        with self.assertLogs(self._logger, logging.INFO) as context:
            self._handler.on_reply(reply)
        self.assertEqual(len(context.output), 1)
        self.assertEqual(task.get_reply(), reply)

    def test_on_error(self) -> None:
        error = brayns.JsonRpcError(0, 1, "test", 123)
        task = self._tasks.create_task(0)
        with self.assertLogs(self._logger, logging.INFO) as context:
            self._handler.on_error(error)
        self.assertEqual(len(context.output), 1)
        with self.assertRaises(brayns.JsonRpcError) as context:
            task.get_reply()
        self.assertEqual(context.exception, error)

    def test_on_error_general(self) -> None:
        error = brayns.JsonRpcError.general("test")
        tasks = [self._tasks.create_task(i) for i in range(3)]
        self._handler.on_error(error)
        for task in tasks:
            with self.assertRaises(brayns.JsonRpcError) as context:
                task.get_reply()
            self.assertEqual(context.exception, error)

    def test_on_progress(self) -> None:
        progress = brayns.JsonRpcProgress(0, "test", 0.5)
        task = self._tasks.create_task(progress.id)
        with self.assertLogs(self._logger, logging.INFO) as context:
            self._handler.on_progress(progress)
        self.assertEqual(len(context.output), 1)
        self.assertEqual(task.get_progress(), progress)

    def test_on_invalid_message(self) -> None:
        with self.assertLogs(self._logger, logging.ERROR) as context:
            self._handler.on_invalid_message(Exception())
        self.assertEqual(len(context.output), 1)
