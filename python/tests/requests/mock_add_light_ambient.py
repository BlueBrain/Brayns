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
    'description': 'Add an ambient light and return its ID',
    'params': {
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
            'is_visible'
        ],
        'title': 'AmbientLight',
        'type': 'object'
    },
    'plugin': 'Core',
    'returns': {
        'minimum': 0,
        'type': 'integer'
    },
    'title': 'add-light-ambient',
    'type': 'method'
}

params = {
    'color': [
        1,
        0,
        0
    ],
    'intensity': 0.75,
    'is_visible': True
}

result = {
    'check': 0.6527030302256398
}
