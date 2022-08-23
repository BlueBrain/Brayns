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

import brayns

from ...simple_test_case import SimpleTestCase


class TestGetApplication(SimpleTestCase):

    def test_get_application(self) -> None:
        application = brayns.get_application(self.instance)
        self.assertSetEqual(set(application.plugins), set(
            plugin.value for plugin in brayns.Plugin
        ))
        self.assertIsInstance(application.resolution, brayns.Resolution)
        self.assertIsInstance(application.jpeg_quality, int)
