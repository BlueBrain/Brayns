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
    'description': 'Get the properties of the current camera',
    'params': [],
    'plugin': 'Circuit Explorer',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'aperture_radius': {
                'description': 'The camera aperture',
                'type': 'number'
            },
            'direction': {
                'description': 'Camera facing direction normalized',
                'items': {
                    'type': 'number'
                },
                'maxItems': 3,
                'minItems': 3,
                'type': 'array'
            },
            'focus_distance': {
                'description': 'Focus distance from the origin',
                'type': 'number'
            },
            'origin': {
                'description': 'Camera position',
                'items': {
                    'type': 'number'
                },
                'maxItems': 3,
                'minItems': 3,
                'type': 'array'
            },
            'up': {
                'description': 'Camera up direction normalized',
                'items': {
                    'type': 'number'
                },
                'maxItems': 3,
                'minItems': 3,
                'type': 'array'
            }
        },
        'required': [
            'origin',
            'direction',
            'up',
            'aperture_radius',
            'focus_distance'
        ],
        'title': 'OduCameraMessage',
        'type': 'object'
    },
    'title': 'get-odu-camera',
    'type': 'method'
}

params = None

result = {
    'check': 0.7883105868668563
}
