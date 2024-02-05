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
    'description': 'Add a visual 3D box to the scene',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'color': {
                    'description': 'Box color RGBA normalized',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 4,
                    'minItems': 4,
                    'type': 'array'
                },
                'max_corner': {
                    'description': 'Axis aligned maximum bound of the box',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'min_corner': {
                    'description': 'Axis aligned minimum bound of the box',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'name': {
                    'description': 'Name to give to the added model',
                    'type': 'string'
                }
            },
            'required': [
                'min_corner',
                'max_corner',
                'color'
            ],
            'title': 'AddBoxMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'id': {
                'description': 'ID of the model of the shape added',
                'minimum': 0,
                'type': 'integer'
            }
        },
        'required': [
            'id'
        ],
        'title': 'AddShapeMessage',
        'type': 'object'
    },
    'title': 'add-box',
    'type': 'method'
}

params = {
    'min_corner': [
        -0.2,
        -0.2,
        -0.2
    ],
    'max_corner': [
        0.2,
        0.2,
        0.2
    ],
    'color': [
        1,
        0,
        0,
        1
    ],
    'name': 'test'
}

result = {
    'check': 0.10267644321697056
}
