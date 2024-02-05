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
    'description': 'Update a clip plane with the given coefficients',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'id': {
                    'description': 'Plane ID',
                    'minimum': 0,
                    'type': 'integer'
                },
                'plane': {
                    'description': 'Plane normal vector XYZ and distance from origin',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 4,
                    'minItems': 4,
                    'type': 'array'
                }
            },
            'title': 'ClipPlane',
            'type': 'object'
        }
    ],
    'plugin': 'Core',
    'returns': {},
    'title': 'update-clip-plane',
    'type': 'method'
}

params = {
    'id': 0,
    'plane': [
        0.5,
        0.5,
        0.5,
        0
    ]
}

result = {
    'check': 0.7074928735227722
}
