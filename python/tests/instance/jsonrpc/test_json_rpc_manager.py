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

import json
import logging
import unittest

import brayns
from brayns.instance.jsonrpc.json_rpc_manager import JsonRpcManager


class TestJsonRpcManager(unittest.TestCase):

    def setUp(self) -> None:
        logger = logging.root
        self._manager = JsonRpcManager(logger)

    def test_is_running(self) -> None:
        self._manager.create_task(0)
        self.assertTrue(self._manager.is_running(0))
        self.assertFalse(self._manager.is_running(1))

    def test_clear(self) -> None:
        task = self._manager.create_task(0)
        self._manager.clear()
        with self.assertRaises(brayns.RequestError):
            task.get_result()
        self.assertFalse(self._manager.is_running(0))

    def test_create_task(self) -> None:
        self._manager.create_task(0)
        self.assertTrue(self._manager.is_running(0))

    def test_process_message(self) -> None:
        task = self._manager.create_task(0)
        reply = {
            'id': 0,
            'result': 123
        }
        self._manager.process_message(json.dumps(reply))
        self.assertEqual(task.get_result(), 123)
        self.assertFalse(self._manager.is_running(0))


if __name__ == '__main__':
    unittest.main()
