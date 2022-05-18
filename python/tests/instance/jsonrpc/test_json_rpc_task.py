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

from brayns.instance.jsonrpc.json_rpc_task import JsonRpcTask
from brayns.instance.request_error import RequestError
from brayns.instance.request_progress import RequestProgress


class TestJsonRpcTask(unittest.TestCase):

    def test_from_result(self) -> None:
        result = 123
        task = JsonRpcTask.from_result(result)
        self.assertEqual(task.get_result(), result)

    def test_init(self) -> None:
        task = JsonRpcTask()
        self.assertFalse(task.is_ready())
        with self.assertRaises(RuntimeError):
            task.get_result()

    def test_is_ready(self) -> None:
        task = JsonRpcTask()
        self.assertFalse(task.is_ready())
        task = JsonRpcTask()
        task.set_result(123)
        self.assertTrue(task.is_ready())
        task = JsonRpcTask()
        task.set_error(RequestError(0, 'test'))
        self.assertTrue(task.is_ready())

    def test_has_progress(self) -> None:
        task = JsonRpcTask()
        self.assertFalse(task.has_progress())
        progress = RequestProgress('test', 0.5)
        task.add_progress(progress)
        self.assertTrue(task.has_progress())

    def test_get_result(self) -> None:
        task = JsonRpcTask()
        with self.assertRaises(RuntimeError):
            task.get_result()
        result = 123
        task.set_result(result)
        self.assertEqual(task.get_result(), result)
        task = JsonRpcTask()
        error = RequestError('test', 0, 123)
        task.set_error(error)
        with self.assertRaises(RequestError) as context:
            task.get_result()
        self.assertEqual(context.exception, error)

    def test_set_result(self) -> None:
        task = JsonRpcTask()
        result = 123
        task.set_result(result)
        self.assertEqual(task.get_result(), result)
        with self.assertRaises(RuntimeError):
            task.set_result(result)

    def test_set_error(self) -> None:
        task = JsonRpcTask()
        error = RequestError('test', 1, 123)
        task.set_error(error)
        with self.assertRaises(RequestError) as context:
            task.get_result()
        self.assertEqual(context.exception, error)
        with self.assertRaises(RuntimeError):
            task.set_error(error)

    def test_get_progress(self) -> None:
        task = JsonRpcTask()
        with self.assertRaises(RuntimeError):
            task.get_progress()
        progress = RequestProgress('test', 0.5)
        task.add_progress(progress)
        self.assertEqual(task.get_progress(), progress)

    def test_add_progress(self) -> None:
        task = JsonRpcTask()
        progress = RequestProgress('test', 0.5)
        for _ in range(3):
            task.add_progress(progress)
        for _ in range(3):
            self.assertEqual(task.get_progress(), progress)


if __name__ == '__main__':
    unittest.main()
