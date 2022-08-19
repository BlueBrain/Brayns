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

import logging
import unittest

import brayns


class TestServiceLauncher(unittest.TestCase):

    def test_uri(self) -> None:
        launcher = brayns.ServiceLauncher(5000)
        self.assertEqual(launcher.uri, 'localhost:5000')

    def test_create_launcher(self) -> None:
        launcher = brayns.ServiceLauncher(
            port=5000,
            server_ssl=brayns.SslServerContext(),
            server_log_level=brayns.LogLevel.INFO,
            server_plugins=['1', '2'],
            server_executable='exec',
            server_env={'test1': '1'},
        )
        ref = brayns.Launcher(
            uri=launcher.uri,
            ssl_context=launcher.server_ssl,
            log_level=launcher.server_log_level,
            plugins=launcher.server_plugins,
            executable=launcher.server_executable,
            env=launcher.server_env,
        )
        test = launcher.create_launcher()
        self.assertEqual(test, ref)

    def test_create_connector(self) -> None:
        def binary_handler(_): return None
        logger = logging.Logger('test')
        launcher = brayns.ServiceLauncher(
            port=5000,
            client_ssl=brayns.SslClientContext(),
            client_binary_handler=binary_handler,
            client_logger=logger,
        )
        ref = brayns.Connector(
            uri=launcher.uri,
            ssl_context=launcher.client_ssl,
            binary_handler=launcher.client_binary_handler,
            logger=launcher.client_logger,
            max_attempts=None,
        )
        test = launcher.create_connector()
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
