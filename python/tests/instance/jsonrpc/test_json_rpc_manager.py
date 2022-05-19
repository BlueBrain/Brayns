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

from brayns.instance.jsonrpc.json_rpc_manager import JsonRpcManager
from brayns.instance.request_error import RequestError
from brayns.instance.request_progress import RequestProgress


class TestJsonRpcManager(unittest.TestCase):

    def setUp(self) -> None:
        self._manager = JsonRpcManager()

    def test_len(self) -> None:
        self.assertEqual(len(self._manager), 0)
        self._manager.add_task(0)
        self._manager.add_task('0')
        self.assertEqual(len(self._manager), 2)

    def test_iter(self) -> None:
        tasks = {id: self._manager.add_task(id) for id in range(3)}
        for id, task in self._manager:
            self.assertFalse(task.is_ready())
            self.assertIn(id, tasks)

    def test_contains(self) -> None:
        self._manager.add_task(0)
        self.assertIn(0, self._manager)

    def test_get_task(self) -> None:
        self._manager.add_task(0)
        self.assertIsNotNone(self._manager.get_task(0))
        self.assertIsNone(self._manager.get_task(1))

    def test_add_task(self) -> None:
        task = self._manager.add_task(0)
        self.assertFalse(task.is_ready())

    def test_cancel_all_tasks(self) -> None:
        tasks = [self._manager.add_task(i) for i in range(3)]
        self._manager.cancel_all_tasks()
        self.assertEqual(len(self._manager), 0)
        for task in tasks:
            with self.assertRaises(RequestError):
                task.get_result()

    def test_set_result(self) -> None:
        result = 123
        task = self._manager.add_task(0)
        self._manager.set_result(0, result)
        self.assertEqual(len(self._manager), 0)
        self.assertEqual(task.get_result(), result)

    def test_set_error(self) -> None:
        error = RequestError('test', 0, 123)
        task = self._manager.add_task(0)
        self._manager.set_error(0, error)
        self.assertEqual(len(self._manager), 0)
        with self.assertRaises(RequestError) as context:
            task.get_result()
        self.assertEqual(context.exception, error)

    def test_add_progress(self) -> None:
        progress = RequestProgress('test', 0.5)
        task = self._manager.add_task(0)
        self._manager.add_progress(0, progress)
        self.assertTrue(task.has_progress())
        self.assertEqual(task.get_progress(), progress)


if __name__ == '__main__':
    unittest.main()
