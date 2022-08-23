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


class TestBundle(unittest.TestCase):

    def test_service_uri(self) -> None:
        bundle = brayns.Bundle(5000, service_host='test')
        self.assertEqual(bundle.service_uri, 'test:5000')

    def test_connector_uri(self) -> None:
        bundle = brayns.Bundle(5000)
        self.assertEqual(bundle.connector_uri, 'localhost:5000')

    def test_create_service(self) -> None:
        bundle = brayns.Bundle(
            port=5000,
            service_host='test',
            service_ssl=brayns.SslServerContext(),
            service_log_level=brayns.LogLevel.INFO,
            service_plugins=['1', '2'],
            service_executable='exec',
            service_env={'test1': '1'},
        )
        ref = brayns.Service(
            uri=bundle.service_uri,
            ssl_context=bundle.service_ssl,
            log_level=bundle.service_log_level,
            plugins=bundle.service_plugins,
            executable=bundle.service_executable,
            env=bundle.service_env,
        )
        test = bundle.create_service()
        self.assertEqual(test, ref)

    def test_create_connector(self) -> None:
        def binary_handler(_): return None
        logger = logging.Logger('test')
        bundle = brayns.Bundle(
            port=5000,
            connector_ssl=brayns.SslClientContext(),
            connector_binary_handler=binary_handler,
            connector_logger=logger,
        )
        ref = brayns.Connector(
            uri=bundle.connector_uri,
            ssl_context=bundle.connector_ssl,
            binary_handler=bundle.connector_binary_handler,
            logger=bundle.connector_logger,
            max_attempts=None,
        )
        test = bundle.create_connector()
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
