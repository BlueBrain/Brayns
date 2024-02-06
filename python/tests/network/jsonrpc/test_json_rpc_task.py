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

import unittest

import brayns
from brayns.network.jsonrpc import JsonRpcTask

from .mock_messages import mock_error, mock_progress, mock_reply


class TestJsonRpcTask(unittest.TestCase):
    def test_from_reply(self) -> None:
        reply = mock_reply()
        task = JsonRpcTask.from_reply(reply)
        self.assertEqual(task.get_reply(), reply)

    def test_init(self) -> None:
        task = JsonRpcTask()
        self.assertFalse(task.is_ready())
        with self.assertRaises(RuntimeError):
            task.get_reply()

    def test_is_ready(self) -> None:
        task = JsonRpcTask()
        self.assertFalse(task.is_ready())
        task = JsonRpcTask()
        task.set_reply(mock_reply())
        self.assertTrue(task.is_ready())
        task = JsonRpcTask()
        task.set_error(mock_error())
        self.assertTrue(task.is_ready())

    def test_has_progress(self) -> None:
        task = JsonRpcTask()
        self.assertFalse(task.has_progress())
        task.add_progress(mock_progress())
        self.assertTrue(task.has_progress())

    def test_get_result(self) -> None:
        task = JsonRpcTask()
        with self.assertRaises(RuntimeError):
            task.get_reply()
        reply = mock_reply()
        task.set_reply(reply)
        self.assertEqual(task.get_reply(), reply)
        task = JsonRpcTask()
        error = mock_error()
        task.set_error(error)
        with self.assertRaises(brayns.JsonRpcError) as context:
            task.get_reply()
        self.assertEqual(context.exception, error)

    def test_set_result(self) -> None:
        task = JsonRpcTask()
        reply = mock_reply()
        task.set_reply(reply)
        self.assertEqual(task.get_reply(), reply)
        with self.assertRaises(RuntimeError):
            task.set_reply(reply)

    def test_set_error(self) -> None:
        task = JsonRpcTask()
        error = mock_error()
        task.set_error(error)
        with self.assertRaises(brayns.JsonRpcError) as context:
            task.get_reply()
        self.assertEqual(context.exception, error)
        with self.assertRaises(RuntimeError):
            task.set_error(error)

    def test_get_progress(self) -> None:
        task = JsonRpcTask()
        with self.assertRaises(RuntimeError):
            task.get_progress()
        progress = mock_progress()
        task.add_progress(progress)
        self.assertEqual(task.get_progress(), progress)

    def test_add_progress(self) -> None:
        task = JsonRpcTask()
        progress = mock_progress()
        for _ in range(3):
            task.add_progress(progress)
        for _ in range(3):
            self.assertEqual(task.get_progress(), progress)
