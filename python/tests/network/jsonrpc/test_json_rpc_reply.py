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

from brayns.network.jsonrpc import (
    deserialize_reply,
    deserialize_reply_from_binary,
    deserialize_reply_from_text,
)

from .mock_messages import (
    mock_reply,
    mock_reply_binary,
    mock_reply_message,
    mock_reply_text,
)


class TestJsonRpcReply(unittest.TestCase):
    def test_deserialize_reply(self) -> None:
        test = deserialize_reply(mock_reply_message())
        self.assertEqual(test, mock_reply())

    def test_deserialize_reply_from_text(self) -> None:
        test = deserialize_reply_from_text(mock_reply_text())
        self.assertEqual(test, mock_reply())

    def test_deserialize_reply_from_binary(self) -> None:
        test = deserialize_reply_from_binary(mock_reply_binary())
        self.assertEqual(test, mock_reply(binary=True))
