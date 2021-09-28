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

schema = {
    'async': False,
    'description': 'Get the current state of the camera',
    'params': [],
    'plugin': 'Core',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'current': {
                'description': 'Camera current type',
                'type': 'string'
            },
            'orientation': {
                'description': 'Camera orientation XYZW',
                'items': {
                    'type': 'number'
                },
                'maxItems': 4,
                'minItems': 4,
                'type': 'array'
            },
            'position': {
                'description': 'Camera position XYZ',
                'items': {
                    'type': 'number'
                },
                'maxItems': 3,
                'minItems': 3,
                'type': 'array'
            },
            'target': {
                'description': 'Camera target XYZ',
                'items': {
                    'type': 'number'
                },
                'maxItems': 3,
                'minItems': 3,
                'type': 'array'
            },
            'types': {
                'description': 'Available camera types',
                'items': {
                    'type': 'string'
                },
                'readOnly': True,
                'type': 'array'
            }
        },
        'title': 'Camera',
        'type': 'object'
    },
    'title': 'get-camera',
    'type': 'method'
}

result = {
    'current': 'perspective',
    'orientation': [0, 0, 0, 1],
    'position': [0, 0, 1],
    'target': [0, 0, 0],
    'types': [
        'circuit_explorer_dof_perspective',
        'circuit_explorer_sphere_clipping',
        'fisheye',
        'orthographic',
        'panoramic',
        'perspective',
        'perspectiveParallax'
    ]
}
