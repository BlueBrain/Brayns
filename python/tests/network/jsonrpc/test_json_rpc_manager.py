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
import unittest
from typing import cast

import brayns
from brayns.network.jsonrpc import JsonRpcManager

from .mock_messages import mock_reply, mock_reply_binary, mock_reply_text


class TestJsonRpcManager(unittest.TestCase):
    def test_is_running(self) -> None:
        manager = self._create_manager()
        manager.create_task(0)
        self.assertTrue(manager.is_running(0))
        self.assertFalse(manager.is_running(1))

    def test_clear(self) -> None:
        manager = self._create_manager()
        task = manager.create_task(0)
        manager.clear()
        with self.assertRaises(brayns.JsonRpcError):
            task.get_reply()
        self.assertFalse(manager.is_running(0))

    def test_create_task(self) -> None:
        manager = self._create_manager()
        manager.create_task(0)
        self.assertTrue(manager.is_running(0))

    def test_process_binary(self) -> None:
        manager = self._create_manager()
        reply = mock_reply(binary=True)
        id = cast(int, reply.id)
        task = manager.create_task(id)
        data = mock_reply_binary()
        manager.process_binary(data)
        self.assertEqual(task.get_reply(), reply)
        self.assertFalse(manager.is_running(0))

    def test_process_text(self) -> None:
        manager = self._create_manager()
        reply = mock_reply()
        id = cast(int, reply.id)
        task = manager.create_task(id)
        data = mock_reply_text()
        manager.process_text(data)
        self.assertEqual(task.get_reply(), reply)
        self.assertFalse(manager.is_running(0))

    def _create_manager(self) -> JsonRpcManager:
        logger = logging.Logger("test")
        return JsonRpcManager(logger)
