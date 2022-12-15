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

from brayns.network.jsonrpc import JsonRpcDispatcher

from .messages.mock_error import MockError
from .messages.mock_progress import MockProgress
from .messages.mock_reply import MockReply
from .mock_json_rpc_listener import MockJsonRpcListener


class TestJsonRpcDispatcher(unittest.TestCase):
    def setUp(self) -> None:
        self._listener = MockJsonRpcListener()
        self._dispatcher = JsonRpcDispatcher(self._listener)

    def test_dispatch_binary(self) -> None:
        data = MockReply.binary
        self._dispatcher.dispatch_binary(data)
        test = self._listener.get_data()
        self.assertEqual(test, MockReply.binary_reply)

    def test_dispatch_text_reply(self) -> None:
        data = MockReply.text
        self._dispatcher.dispatch_text(data)
        test = self._listener.get_data()
        self.assertEqual(test, MockReply.reply)

    def test_dispatch_text_error(self) -> None:
        data = MockError.data
        self._dispatcher.dispatch_text(data)
        test = self._listener.get_data()
        self.assertEqual(test, MockError.error)

    def test_dispatch_text_progress(self) -> None:
        data = MockProgress.data
        self._dispatcher.dispatch_text(data)
        test = self._listener.get_data()
        self.assertEqual(test, MockProgress.progress)

    def test_dispatch_text_invalid_frame(self) -> None:
        data = "sdfbxcbxbcv"
        self._dispatcher.dispatch_text(data)
        e = self._listener.get_data()
        self.assertIsInstance(e, Exception)
