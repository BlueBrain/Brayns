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


class TestServiceCli(unittest.TestCase):

    def test_parse(self) -> None:
        test = brayns.ServiceCli(
            port=3,
            executable='exe',
            library_path='osp',
        )
        args = [
            '--service_port',
            '4',
            '--service_executable',
            'exe2',
            '--library_path',
            'osp2',
        ]
        test.parse(args)
        self.assertEqual(test.port, 4)
        self.assertEqual(test.executable, 'exe2')
        self.assertEqual(test.library_path, 'osp2')

    def test_create_bundle(self) -> None:
        cli = brayns.ServiceCli(
            port=3,
            executable='exe',
            library_path='osp',
        )
        test = cli.create_bundle()
        ref = brayns.Bundle(
            port=cli.port,
            service_executable=cli.executable,
            service_env={'LD_LIBRARY_PATH': cli.library_path},
            connector_logger=test.connector_logger,
        )
        self.assertEqual(test, ref)


if __name__ == '__main__':
    unittest.main()
