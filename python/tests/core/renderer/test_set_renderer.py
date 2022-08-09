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

from brayns.core.renderer.production_renderer import ProductionRenderer
from brayns.core.renderer.set_renderer import set_renderer
from tests.instance.mock_instance import MockInstance


class TestSetRenderer(unittest.TestCase):

    def test_set_renderer(self) -> None:
        instance = MockInstance()
        renderer = ProductionRenderer()
        set_renderer(instance, renderer)
        self.assertEqual(instance.method, 'set-renderer-production')
        self.assertEqual(instance.params, renderer.serialize())


if __name__ == '__main__':
    unittest.main()