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


class TestRendererCli(unittest.TestCase):

    def test_parse(self) -> None:
        test = brayns.RendererCli(
            name=brayns.ProductionRenderer.name,
            available=[brayns.ProductionRenderer],
            background=brayns.Color4.red,
            samples=2,
        )
        args = [
            '--renderer_type',
            'production',
            '--background',
            '0',
            '1',
            '0',
            '0',
            '--samples',
            '3',
        ]
        test.parse(args)
        self.assertEqual(test.name, 'production')
        self.assertEqual(test.background, brayns.Color4.green.transparent)
        self.assertEqual(test.samples, 3)

    def test_create_renderer(self) -> None:
        cli = brayns.RendererCli(
            name=brayns.ProductionRenderer.name,
            background=brayns.Color4.red,
            samples=3,
        )
        test = cli.create_renderer()
        ref = brayns.ProductionRenderer(
            samples_per_pixel=cli.samples,
            background_color=cli.background,
        )
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
