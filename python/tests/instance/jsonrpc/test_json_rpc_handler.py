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

import logging
import sys
import unittest

from brayns.instance.jsonrpc.json_rpc_error import JsonRpcError
from brayns.instance.jsonrpc.json_rpc_handler import JsonRpcHandler
from brayns.instance.jsonrpc.json_rpc_manager import JsonRpcManager
from brayns.instance.jsonrpc.json_rpc_progress import JsonRpcProgress
from brayns.instance.jsonrpc.json_rpc_reply import JsonRpcReply
from brayns.instance.request_error import RequestError
from brayns.instance.request_progress import RequestProgress


class TestJsonRpcHandler(unittest.TestCase):

    def setUp(self) -> None:
        self._logger = logging.Logger('Test', logging.CRITICAL)
        self._logger.addHandler(logging.StreamHandler(sys.stdout))
        self._manager = JsonRpcManager()
        self._handler = JsonRpcHandler(self._manager, self._logger)

    def test_on_binary(self) -> None:
        with self.assertLogs(self._logger, logging.INFO) as context:
            self._handler.on_binary(b'test')
        self.assertEqual(len(context.output), 1)

    def test_on_reply(self) -> None:
        reply = JsonRpcReply(0, 123)
        task = self._manager.add_task(reply.id)
        with self.assertLogs(self._logger, logging.INFO) as context:
            self._handler.on_reply(reply)
        self.assertEqual(len(context.output), 1)
        self.assertEqual(len(self._manager), 0)
        self.assertEqual(task.get_result(), reply.result)

    def test_on_error(self) -> None:
        error = JsonRpcError(0, RequestError('test', 0, 123))
        task = self._manager.add_task(error.id)
        with self.assertLogs(self._logger, logging.INFO) as context:
            self._handler.on_error(error)
        self.assertEqual(len(context.output), 1)
        self.assertEqual(len(self._manager), 0)
        with self.assertRaises(RequestError) as context:
            task.get_result()

    def test_on_error_global(self) -> None:
        error = JsonRpcError(None, RequestError(0, 'test'))
        for i in range(3):
            self._manager.add_task(i)
        self._handler.on_error(error)
        self.assertEqual(len(self._manager), 0)

    def test_on_progress(self) -> None:
        progress = JsonRpcProgress(0, RequestProgress('test', 0.5))
        self._manager.add_task(progress.id)
        with self.assertLogs(self._logger, logging.INFO) as context:
            self._handler.on_progress(progress)
        self.assertEqual(len(context.output), 1)
        self.assertEqual(len(self._manager), 1)

    def test_on_invalid_frame(self) -> None:
        with self.assertLogs(self._logger, logging.ERROR) as context:
            self._handler.on_invalid_frame('test', Exception())
        self.assertEqual(len(context.output), 1)


if __name__ == '__main__':
    unittest.main()
