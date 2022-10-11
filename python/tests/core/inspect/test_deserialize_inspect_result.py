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

from brayns.core import deserialize_inspect_result

from .mock_inspect_result import MockInspectResult


class TestDeserializeInspectResult(unittest.TestCase):

    def test_deserialize_inspect_result(self) -> None:
        message = MockInspectResult.create_message(hit=True)
        test = deserialize_inspect_result(message)
        self.assertEqual(test, MockInspectResult.result)

    def test_deserialize_inspect_result_no_hit(self) -> None:
        message = MockInspectResult.create_message(hit=False)
        test = deserialize_inspect_result(message)
        self.assertIsNone(test)


if __name__ == '__main__':
    unittest.main()
