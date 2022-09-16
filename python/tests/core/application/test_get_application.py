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
from tests.mock_instance import MockInstance


class TestGetApplication(unittest.TestCase):

    def test_get_application(self) -> None:
        instance = MockInstance({
            'plugins': ['test1', 'test2'],
            'viewport': [100, 200],
            'jpeg_quality': 50,
        })
        test = brayns.get_application(instance)
        self.assertEqual(test.plugins, ['test1', 'test2'])
        self.assertEqual(test.resolution, brayns.Resolution(100, 200))
        self.assertEqual(test.jpeg_quality, 50)
        self.assertEqual(instance.method, 'get-application-parameters')
        self.assertEqual(instance.params, None)


if __name__ == '__main__':
    unittest.main()
