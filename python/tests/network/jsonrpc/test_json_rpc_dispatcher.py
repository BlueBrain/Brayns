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
from typing import Any

import brayns
from brayns.network.jsonrpc import JsonRpcDispatcher, JsonRpcListener

from .mock_messages import (
    mock_error,
    mock_error_data,
    mock_progress,
    mock_progress_data,
    mock_reply,
    mock_reply_binary,
    mock_reply_text,
)


class MockJsonRpcListener(JsonRpcListener):
    def __init__(self) -> None:
        self._called = False
        self._data = None

    def get_data(self) -> Any:
        if not self._called:
            raise RuntimeError("Data not received")
        return self._data

    def on_reply(self, reply: brayns.JsonRpcReply) -> None:
        self._set_data(reply)

    def on_error(self, error: brayns.JsonRpcError) -> None:
        self._set_data(error)

    def on_progress(self, progress: brayns.JsonRpcProgress) -> None:
        self._set_data(progress)

    def on_invalid_message(self, e: Exception) -> None:
        self._set_data(e)

    def _set_data(self, data: Any) -> None:
        if self._called:
            raise RuntimeError("Data received twice")
        self._called = True
        self._data = data


class TestJsonRpcDispatcher(unittest.TestCase):
    def setUp(self) -> None:
        self._listener = MockJsonRpcListener()
        self._dispatcher = JsonRpcDispatcher(self._listener)

    def test_dispatch_binary(self) -> None:
        data = mock_reply_binary()
        self._dispatcher.dispatch_binary(data)
        test = self._listener.get_data()
        self.assertEqual(test, mock_reply(binary=True))

    def test_dispatch_text_reply(self) -> None:
        data = mock_reply_text()
        self._dispatcher.dispatch_text(data)
        test = self._listener.get_data()
        self.assertEqual(test, mock_reply())

    def test_dispatch_text_error(self) -> None:
        data = mock_error_data()
        self._dispatcher.dispatch_text(data)
        test = self._listener.get_data()
        self.assertEqual(test, mock_error())

    def test_dispatch_text_progress(self) -> None:
        data = mock_progress_data()
        self._dispatcher.dispatch_text(data)
        test = self._listener.get_data()
        self.assertEqual(test, mock_progress())

    def test_dispatch_text_invalid_frame(self) -> None:
        data = "sdfbxcbxbcv"
        self._dispatcher.dispatch_text(data)
        e = self._listener.get_data()
        self.assertIsInstance(e, Exception)
