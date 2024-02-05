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
    'description': 'Color cells with given colors using their GID',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'colors': {
                    'description': 'Cell colors',
                    'items': {
                        'type': 'number'
                    },
                    'type': 'array'
                },
                'gids': {
                    'description': 'Cells to color',
                    'items': {
                        'type': 'string'
                    },
                    'type': 'array'
                },
                'model_id': {
                    'description': 'Model to color',
                    'minimum': 0,
                    'type': 'integer'
                }
            },
            'required': [
                'model_id',
                'gids',
                'colors'
            ],
            'title': 'ColorCellsMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'color-cells',
    'type': 'method'
}

params = {
    'model_id': 0,
    'gids': [
        '0,1-3',
        '4-7'
    ],
    'colors': [
        1,
        0,
        0,
        0,
        1,
        0
    ]
}

result = {
    'check': 0.557867740522802
}
