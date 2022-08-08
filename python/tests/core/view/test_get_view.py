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

from brayns.core.view.get_view import get_view
from tests.core.view.mock_view import MockView
from tests.instance.mock_instance import MockInstance


class TestGetView(unittest.TestCase):

    def test_get_view(self) -> None:
        instance = MockInstance(MockView.message)
        view = get_view(instance)
        self.assertEqual(instance.method, 'get-camera-look-at')
        self.assertEqual(instance.params, None)
        self.assertEqual(view, MockView.view)


if __name__ == '__main__':
    unittest.main()
