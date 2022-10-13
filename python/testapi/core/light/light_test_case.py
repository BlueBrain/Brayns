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
from testapi.simple_test_case import SimpleTestCase


class LightTestCase(SimpleTestCase):

    def run_tests(self, light: brayns.Light) -> None:
        tests = [
            brayns.add_light(self.instance, light)
            for _ in range(3)
        ]
        refs = [
            brayns.get_model(self.instance, model.id)
            for model in tests
        ]
        self.assertEqual(tests, refs)
        remaining = tests[0].id
        removed = [model.id for model in tests[1:]]
        brayns.remove_models(self.instance, removed)
        for id in removed:
            with self.assertRaises(brayns.JsonRpcError):
                brayns.get_model(self.instance, id)
        brayns.get_model(self.instance, remaining)
        brayns.clear_lights(self.instance)
        with self.assertRaises(brayns.JsonRpcError):
            brayns.get_model(self.instance, remaining)
