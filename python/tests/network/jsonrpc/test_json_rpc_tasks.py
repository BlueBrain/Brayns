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
from typing import cast

import brayns
from brayns.network.jsonrpc import JsonRpcTask, JsonRpcTasks

from .mock_messages import mock_error, mock_progress, mock_reply


class TestJsonRpcTasks(unittest.TestCase):
    def test_len(self) -> None:
        tasks = JsonRpcTasks()
        for i in range(10):
            tasks.create_task(i)
        self.assertEqual(len(tasks), 10)

    def test_iter(self) -> None:
        tasks = JsonRpcTasks()
        for i in range(10):
            tasks.create_task(i)
        ids = [id for id, _ in tasks]
        self.assertEqual(ids, list(range(10)))
        for _, task in tasks:
            self.assertIsInstance(task, JsonRpcTask)

    def test_contains(self) -> None:
        tasks = JsonRpcTasks()
        tasks.create_task(0)
        self.assertIn(0, tasks)

    def test_find(self) -> None:
        tasks = JsonRpcTasks()
        tasks.create_task(0)
        self.assertIsNotNone(tasks.find(0))
        self.assertIsNone(tasks.find(1))

    def test_create_task(self) -> None:
        tasks = JsonRpcTasks()
        task = tasks.create_task(0)
        self.assertFalse(task.is_ready())

    def test_add_result(self) -> None:
        tasks = JsonRpcTasks()
        reply = mock_reply()
        id = cast(int, reply.id)
        task = tasks.create_task(id)
        tasks.add_reply(reply)
        self.assertEqual(task.get_reply(), reply)

    def test_add_error(self) -> None:
        tasks = JsonRpcTasks()
        error = mock_error()
        id = cast(int, error.id)
        task = tasks.create_task(id)
        tasks.add_error(error)
        with self.assertRaises(brayns.JsonRpcError) as context:
            task.get_reply()
        self.assertEqual(context.exception, error)

    def test_add_general_error(self) -> None:
        tasks = JsonRpcTasks()
        error = mock_error()
        ref = [tasks.create_task(i) for i in range(10)]
        tasks._add_general_error(error)
        self.assertEqual(len(tasks), 0)
        for task in ref:
            with self.assertRaises(brayns.JsonRpcError) as context:
                task.get_reply()
            self.assertEqual(context.exception, error)

    def test_add_progress(self) -> None:
        tasks = JsonRpcTasks()
        progress = mock_progress()
        task = tasks.create_task(progress.id)
        tasks.add_progress(progress)
        self.assertTrue(task.has_progress())
        self.assertEqual(task.get_progress(), progress)
