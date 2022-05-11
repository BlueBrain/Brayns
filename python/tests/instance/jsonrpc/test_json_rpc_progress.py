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

from brayns.instance.jsonrpc.json_rpc_progress import JsonRpcProgress


class TestJsonRpcProgress(unittest.TestCase):

    def test_from_dict(self) -> None:
        progress = JsonRpcProgress.from_dict({
            'params': {
                'id': 1,
                'operation': 'test',
                'amount': 0.5
            }
        })
        self.assertEqual(progress.id, 1)
        self.assertEqual(progress.params.operation, 'test')
        self.assertEqual(progress.params.amount, 0.5)


if __name__ == '__main__':
    unittest.main()
