# Copyright (c) 2021 EPFL/Blue Brain Project
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

import pathlib
from typing import List

from mock_request import MockRequest


def load() -> List[MockRequest]:
    return [
        _load_request(path)
        for path in _get_mock_files()
    ]


def _get_mock_files():
    directory = pathlib.Path(__file__).parent / 'requests'
    return directory.glob('mock_*.py')


def _load_request(path: pathlib.Path):
    context = {}
    with path.open() as f:
        exec(f.read(), context)
    return MockRequest(
        schema=context.get('schema'),
        params=context.get('params'),
        result=context.get('result')
    )
