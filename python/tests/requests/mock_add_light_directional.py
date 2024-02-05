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
    'description': 'Add a directional light and return its ID',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'angular_diameter': {
                    'description': 'Angular diameter in degrees',
                    'type': 'number'
                },
                'color': {
                    'description': 'Light color RGB',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'direction': {
                    'description': 'Light source direction',
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
                }
            },
            'required': [
                'color',
                'intensity',
                'is_visible',
                'direction',
                'angular_diameter'
            ],
            'title': 'DirectionalLight',
            'type': 'object'
        }
    ],
    'plugin': 'Core',
    'returns': {
        'minimum': 0,
        'type': 'integer'
    },
    'title': 'add-light-directional',
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
    'direction': [
        0,
        0,
        1
    ],
    'angular_diameter': 1
}

result = {
    'check': 0.09474074260263221
}
