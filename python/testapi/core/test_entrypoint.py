# Copyright (c) 2015-2024 EPFL/Blue Brain Project
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
from testapi.simple_test_case import SimpleTestCase


class TestEntrypoint(SimpleTestCase):
    def test_get_entrypoint(self) -> None:
        method = "snapshot"
        entrypoint = brayns.get_entrypoint(self.instance, method)
        self.assertEqual(entrypoint.method, method)

    def test_get_entrypoints(self) -> None:
        methods = brayns.get_methods(self.instance)
        entrypoints = brayns.get_entrypoints(self.instance)
        self.assertEqual(methods, [entrypoint.method for entrypoint in entrypoints])
