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
import unittest

from brayns.network import deserialize_reply_from_binary, deserialize_reply_from_text


class TestDeserializeReplyFromBinary(unittest.TestCase):
    def test_deserialize_reply_from_binary(self) -> None:
        text = json.dumps(
            {
                "id": 1,
                "result": 123,
            }
        )
        size = len(text).to_bytes(4, byteorder="little", signed=False)
        binary = b"123"
        data = b"".join([size, text.encode("utf-8"), binary])
        test = deserialize_reply_from_binary(data)
        ref = deserialize_reply_from_text(text)
        self.assertEqual(test.id, ref.id)
        self.assertEqual(test.result, ref.result)
        self.assertEqual(test.binary, binary)
