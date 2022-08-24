# Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

import unittest

import brayns

from brayns.network.jsonrpc.json_rpc_task import JsonRpcTask
from brayns.network.jsonrpc.json_rpc_tasks import JsonRpcTasks


class TestJsonRpcTasks(unittest.TestCase):

    def setUp(self) -> None:
        self._tasks = JsonRpcTasks()

    def test_len(self) -> None:
        for i in range(10):
            self._tasks.create_task(i)
        self.assertEqual(len(self._tasks), 10)

    def test_iter(self) -> None:
        for i in range(10):
            self._tasks.create_task(i)
        ids = [id for id, _ in self._tasks]
        self.assertEqual(ids, list(range(10)))
        for _, task in self._tasks:
            self.assertIsInstance(task, JsonRpcTask)

    def test_contains(self) -> None:
        self._tasks.create_task(0)
        self.assertIn(0, self._tasks)

    def test_find(self) -> None:
        self._tasks.create_task(0)
        self.assertIsNotNone(self._tasks.find(0))
        self.assertIsNone(self._tasks.find(1))

    def test_create_task(self) -> None:
        task = self._tasks.create_task(0)
        self.assertFalse(task.is_ready())

    def test_add_result(self) -> None:
        result = 123
        task = self._tasks.create_task(0)
        self._tasks.add_result(0, result)
        self.assertEqual(task.get_result(), result)

    def test_add_error(self) -> None:
        error = brayns.RequestError(0, 'test', 123)
        task = self._tasks.create_task(0)
        self._tasks.add_error(0, error)
        with self.assertRaises(brayns.RequestError) as context:
            task.get_result()
        self.assertEqual(context.exception, error)

    def test_add_global_error(self) -> None:
        error = brayns.RequestError(0, 'test', 123)
        tasks = [self._tasks.create_task(i) for i in range(10)]
        self._tasks.add_global_error(error)
        self.assertEqual(len(self._tasks), 0)
        for task in tasks:
            with self.assertRaises(brayns.RequestError) as context:
                task.get_result()
            self.assertEqual(context.exception, error)

    def test_add_progress(self) -> None:
        progress = brayns.RequestProgress('test', 0.5)
        task = self._tasks.create_task(0)
        self._tasks.add_progress(0, progress)
        self.assertTrue(task.has_progress())
        self.assertEqual(task.get_progress(), progress)


if __name__ == '__main__':
    unittest.main()
