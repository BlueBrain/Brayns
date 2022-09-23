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

import brayns
from brayns.network import serialize_request_as_bytes, serialize_request_as_json


class TestSerializeRequestAsBytes(unittest.TestCase):

    def test_serialize_request_as_bytes(self) -> None:
        request = brayns.Request(0, 'test', 123, b'123')
        test = serialize_request_as_bytes(request)
        ref = serialize_request_as_json(request)
        size = int.from_bytes(test[:4], byteorder='little', signed=False)
        self.assertEqual(size, len(ref))
        text = test[4:size+4].decode('utf-8')
        self.assertEqual(text, ref)
        binary = test[size+4:]
        self.assertEqual(binary, request.binary)


if __name__ == '__main__':
    unittest.main()
