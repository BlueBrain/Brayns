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

from brayns.network import serialize_request_to_binary, serialize_request_to_text

from .mock_request import MockRequest


class TestSerializeRequestToBinary(unittest.TestCase):

    def test_serialize_request_to_binary(self) -> None:
        test = serialize_request_to_binary(MockRequest.binary_request)
        self.assertEqual(test, MockRequest.binary)

    def test_serialize_request_to_binary_no_binary(self) -> None:
        data = serialize_request_to_binary(MockRequest.request)
        test = data[4:].decode('utf-8')
        ref = serialize_request_to_text(MockRequest.request)
        self.assertEqual(test, ref)
        self.assertEqual(test, MockRequest.text)
