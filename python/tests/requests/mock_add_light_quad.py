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

schema = {
    'async': False,
    'description': 'Add a quad light and return its ID',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'color': {
                    'description': 'Light color RGB',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'edge1': {
                    'description': 'First edge XYZ',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'edge2': {
                    'description': 'Second edge XYZ',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'intensity': {
                    'description': 'Light intensity 0-1',
                    'type': 'number'
                },
                'is_visible': {
                    'description': 'Light is visible',
                    'type': 'boolean'
                },
                'position': {
                    'description': 'Light position XYZ',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                }
            },
            'required': [
                'color',
                'intensity',
                'is_visible',
                'position',
                'edge1',
                'edge2'
            ],
            'title': 'QuadLight',
            'type': 'object'
        }
    ],
    'plugin': 'Core',
    'returns': {
        'minimum': 0,
        'type': 'integer'
    },
    'title': 'add-light-quad',
    'type': 'method'
}

params = {
    'color': [
        1,
        0,
        0
    ],
    'intensity': 0.75,
    'is_visible': True,
    'position': [
        0,
        0,
        0
    ],
    'edge1': [
        0,
        1,
        0
    ],
    'edge2': [
        0,
        0,
        1
    ]
}

result = {
    'check': 0.670584967894954
}
