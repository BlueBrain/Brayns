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

from testapi.api_test_case import ApiTestCase


class SimpleTestCase(ApiTestCase):

    @property
    def instance(self) -> brayns.Instance:
        return self.__instance

    def setUp(self) -> None:
        launcher = brayns.Launcher(
            executable=self.executable,
            uri=self.uri,
            env=self.env
        )
        self.__process = launcher.start()
        connector = brayns.Connector(
            uri=self.uri,
            binary_handler=self.on_binary,
            max_attempts=None
        )
        self.__instance = connector.connect()

    def tearDown(self) -> None:
        self.__instance.disconnect()
        self.__process.terminate()

    def on_binary(self, _: bytes) -> None:
        pass
