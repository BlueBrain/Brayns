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


class TestService(unittest.TestCase):

    def test_get_command_line(self) -> None:
        service = brayns.Service(
            uri='uri',
            ssl_context=brayns.SslServerContext(
                private_key_file='private',
                private_key_passphrase='passphrase',
                certificate_file='certificate',
                ca_location='ca',
            ),
            max_clients=2,
            log_level=brayns.LogLevel.CRITICAL,
            plugins=[
                brayns.Plugin.ATLAS_EXPLORER.value,
                brayns.Plugin.CIRCUIT_EXPLORER.value,
            ],
            executable='service',
        )
        test = service.get_command_line()
        ref = [
            'service',
            '--uri',
            'uri',
            '--max-clients',
            '2',
            '--log-level',
            'critical',
            '--plugin',
            brayns.Plugin.ATLAS_EXPLORER.value,
            '--plugin',
            brayns.Plugin.CIRCUIT_EXPLORER.value,
            '--secure',
            'true',
            '--private-key-file',
            'private',
            '--private-key-passphrase',
            'passphrase',
            '--certificate-file',
            'certificate',
            '--ca-location',
            'ca',
        ]
        self.assertEqual(test, ref)
