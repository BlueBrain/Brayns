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

from brayns.client.jsonrpc.json_rpc_manager import JsonRpcManager
from brayns.client.request_error import RequestError
from brayns.client.request_progress import RequestProgress


class TestJsonRpcManager(unittest.TestCase):

    def setUp(self) -> None:
        self._manager = JsonRpcManager()
        self._ids = [1, 2, 3, 'four']
        for id in self._ids:
            self._manager.add_task(id)
        self._tasks = [task for _, task in self._manager]

    def test_add_tasks(self) -> None:
        self.assertEqual(len(self._manager), len(self._ids))
        for id in self._ids:
            self.assertIn(id, self._manager)
            self.assertIsNotNone(self._manager.get_task(id))
        for id, task in self._manager:
            self.assertIn(id, self._ids)
            self.assertFalse(task.is_ready())
            self.assertFalse(task.has_progress())
        with self.assertRaises(RuntimeError):
            self._manager.add_task(self._ids[0])
        with self.assertRaises(RuntimeError):
            self._manager.add_task(None)

    def test_cancel(self) -> None:
        error = RequestError('Test cancel all')
        self._manager.cancel_all_tasks(error)
        self.assertEqual(len(self._manager), 0)
        for task in self._tasks:
            with self.assertRaises(RequestError) as context:
                task.get_result()
            self.assertEqual(context.exception, error)

    def test_result(self) -> None:
        id = self._ids[0]
        task = self._tasks[0]
        result = 3
        self._manager.set_result(id, result)
        self.assertEqual(task.get_result(), result)
        self.assertEqual(len(self._manager), len(self._ids) - 1)
        for id, task in self._manager:
            self.assertFalse(task.is_ready())

    def test_error(self) -> None:
        id = self._ids[0]
        task = self._tasks[0]
        error = RequestError('test')
        self._manager.set_error(id, error)
        with self.assertRaises(RequestError):
            task.get_result()
        self.assertEqual(len(self._manager), len(self._ids) - 1)
        for id, task in self._manager:
            self.assertFalse(task.is_ready())

    def test_progress(self) -> None:
        id = self._ids[0]
        task = self._tasks[0]
        progress = RequestProgress('test', 0.5)
        self._manager.add_progress(id, progress)
        self.assertEqual(task.get_progress(), progress)
        self.assertEqual(len(self._manager), len(self._ids))
        for id, task in self._manager:
            self.assertFalse(task.has_progress())


if __name__ == '__main__':
    unittest.main()
