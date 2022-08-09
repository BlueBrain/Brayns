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

from brayns.core.clipping.remove_clip_planes import remove_clip_planes
from tests.instance.mock_instance import MockInstance


class TestRemoveClipPlanes(unittest.TestCase):

    def test_remove_clip_planes(self) -> None:
        instance = MockInstance()
        ids = [1, 2, 3]
        remove_clip_planes(instance, ids)
        self.assertEqual(instance.method, 'remove-clip-planes')
        self.assertEqual(instance.params, {'ids': ids})


if __name__ == '__main__':
    unittest.main()