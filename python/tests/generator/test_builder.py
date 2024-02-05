# Copyright 2015-2024 Blue Brain Project/EPFL
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

import json
import pathlib
import random
from dataclasses import dataclass
from typing import Any, List, Union

import brayns


def create_mock_requests(
    samples_path: str,
    brayns_uri: str,
    output_folder: str
):
    with brayns.connect(brayns_uri) as client:
        _build_files(
            requests=_extract_requests(
                client=client,
                samples=_extract_samples(samples_path)
            ),
            output_folder=output_folder
        )


_PATTERN = '''# Copyright 2015-2024 Blue Brain Project/EPFL
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

schema = {schema}

params = {params}

result = {result}
'''


@dataclass
class _Request:

    method: str
    schema: dict
    params: Union[Any, None] = None
    result: Union[Any, None] = None


def _build_files(requests: List[_Request], output_folder: str):
    for request in requests:
        _build_file(output_folder, request)


def _build_file(dirname: str, request: _Request):
    method = request.method.replace('-', '_')
    filename = f'mock_{method}.py'
    with (pathlib.Path(dirname) / filename).open('w') as file:
        file.write(_format_request(request))


def _format_request(request: _Request):
    return _PATTERN.format(
        schema=_format_dict(request.schema),
        params=_format_dict(request.params),
        result=_format_dict(request.result)
    )


def _format_dict(obj: Union[dict, None]):
    if obj is None:
        return 'None'
    return (
        json.dumps(obj, indent=4)
        .replace('\'', '\\\'')
        .replace('"', '\'')
        .replace('true', 'True')
        .replace('false', 'False')
        .replace(': null', ': None')
    )


def _extract_requests(client: brayns.Client, samples: dict):
    return [
        _build_request(client, name, sample)
        for name, sample in samples.items()
    ]


def _build_request(client: brayns.Client, name: str, sample: dict):
    method = name.replace('_', '-')
    return _Request(
        method=method,
        schema=client.schema(method),
        params=sample.get('params', None),
        result={'check': random.random()}
    )


def _extract_samples(path: str):
    return {
        name: var
        for name, var in _build_samples(path).items()
        if _is_request(name, var)
    }


def _build_samples(path: str):
    with open(path) as samples:
        context = {}
        exec(samples.read(), context)
    return context


def _is_request(name: str, var: Any):
    return (
        name.islower()
        and not name.startswith('__')
        and isinstance(var, dict)
    )
