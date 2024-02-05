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

schema = {
    'async': False,
    'description': 'Mirrors a model along a given axis',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'mirror_axis': {
                    'description': 'The axis used to mirror',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'model_id': {
                    'description': 'Model to mirror',
                    'minimum': 0,
                    'type': 'integer'
                }
            },
            'required': [
                'model_id',
                'mirror_axis'
            ],
            'title': 'MirrorModelMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'mirror-model',
    'type': 'method'
}

params = {
    'model_id': 0,
    'mirror_axis': [
        0,
        1,
        0
    ]
}

result = {
    'check': 0.548542086713237
}
