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

import os
import pathlib
import unittest

import brayns


class ApiTestCase(unittest.TestCase):

    @property
    def executable(self) -> str:
        return os.environ['BRAYNS_TEST_EXECUTABLE']

    @property
    def env(self) -> dict[str, str]:
        result = dict[str, str]()
        ospray = os.environ.get('BRAYNS_TEST_OSPRAY_DIR')
        if ospray is not None:
            result['LD_LIBRARY_PATH'] = ospray
        return result

    @property
    def circuit(self) -> str:
        return os.environ['BRAYNS_TEST_CIRCUIT']

    @property
    def ffmpeg(self) -> str:
        return os.environ.get('BRAYNS_TEST_FFMPEG', 'ffmpeg')

    @property
    def asset_folder(self) -> pathlib.Path:
        testapi = pathlib.Path(__file__).parent
        return testapi / 'assets'

    def create_launcher(self) -> brayns.Launcher:
        return brayns.Launcher(
            executable=self.executable,
            env=self.env
        )
