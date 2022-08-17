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


class TestLauncher(unittest.TestCase):

    def test_get_command_line(self) -> None:
        launcher = brayns.Launcher(
            uri='uri',
            ssl_context=brayns.SslServerContext(),
            log_level=brayns.LogLevel.CRITICAL,
            resolution=brayns.Resolution(12, 23),
            jpeg_quality=25,
            plugins=[
                brayns.Plugin.ATLAS_EXPLORER,
                brayns.Plugin.CIRCUIT_EXPLORER
            ],
            executable='service',
        )
        test = launcher.get_command_line()
        ref = [
            'service',
            '--uri',
            'uri',
            '--log-level',
            'critical',
            '--window-size',
            '12 23',
            '--jpeg-quality',
            '25',
            '--plugin',
            brayns.Plugin.ATLAS_EXPLORER.value,
            '--plugin',
            brayns.Plugin.CIRCUIT_EXPLORER.value,
            '--secure',
            'true',
        ]
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
