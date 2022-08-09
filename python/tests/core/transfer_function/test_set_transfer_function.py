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

from brayns.core.transfer_function.set_transfer_function import set_transfer_function
from tests.core.transfer_function.mock_transfer_function import MockTransferFunction
from tests.instance.mock_instance import MockInstance


class TestSetTransferFunction(unittest.TestCase):

    def test_set_transfer_function(self) -> None:
        instance = MockInstance()
        function = MockTransferFunction.transfer_function
        set_transfer_function(instance, 0, function)
        self.assertEqual(instance.method, 'set-model-transfer-function')
        self.assertEqual(instance.params, {
            'id': 0,
            'transfer_function': MockTransferFunction.message
        })


if __name__ == '__main__':
    unittest.main()
