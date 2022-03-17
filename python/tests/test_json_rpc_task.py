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

from brayns.client.jsonrpc.json_rpc_task import JsonRpcTask
from brayns.client.request_error import RequestError
from brayns.client.request_progress import RequestProgress


class TestJsonRpcTask(unittest.TestCase):

    def test_empty(self) -> None:
        task = JsonRpcTask()
        self.assertFalse(task.is_ready())
        self.assertFalse(task.has_progress())
        with self.assertRaises(RuntimeError):
            task.get_result()
        with self.assertRaises(RuntimeError):
            task.get_progress()

    def test_progress(self) -> None:
        task = JsonRpcTask()
        progress = RequestProgress('test', 0.5)
        task.add_progress(progress)
        self.assertFalse(task.is_ready())
        self.assertTrue(task.has_progress())
        self.assertEqual(task.get_progress(), progress)
        self.assertFalse(task.has_progress())
        task.add_progress(RequestProgress('test', 0.6))
        task.add_progress(progress)
        self.assertTrue(task.has_progress())
        self.assertFalse(task.is_ready())
        self.assertEqual(task.get_progress(), progress)

    def test_result(self) -> None:
        task = JsonRpcTask()
        result = {'test': 22}
        task.set_result(result)
        self.assertTrue(task.is_ready())
        self.assertEqual(task.get_result(), result)
        task = JsonRpcTask.from_result(22)
        self.assertEqual(task.get_result(), 22)

    def test_error(self) -> None:
        task = JsonRpcTask()
        error = RequestError('test', 0, [1, 2, 3])
        task.set_error(error)
        self.assertTrue(task.is_ready())
        with self.assertRaises(RequestError) as context:
            task.get_result()
        self.assertEqual(context.exception.message, error.message)
        self.assertEqual(context.exception.code, error.code)
        self.assertEqual(context.exception.data, error.data)


if __name__ == '__main__':
    unittest.main()
