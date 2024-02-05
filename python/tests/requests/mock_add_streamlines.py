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
    'description': 'Add a streamline representation to the scene',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'color_scheme': {
                    'description': 'Color scheme to draw the streamlines',
                    'minimum': 0,
                    'type': 'integer'
                },
                'gids': {
                    'description': 'List of cell GIDs',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'type': 'array'
                },
                'indices': {
                    'description': 'List of indices',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'type': 'array'
                },
                'name': {
                    'description': 'Model name',
                    'type': 'string'
                },
                'opacity': {
                    'description': 'Color opacity',
                    'type': 'number'
                },
                'radius': {
                    'description': 'Streamline tube radius',
                    'type': 'number'
                },
                'vertices': {
                    'description': 'List of vertices (3 components per vertex)',
                    'items': {
                        'type': 'number'
                    },
                    'type': 'array'
                }
            },
            'required': [
                'name',
                'gids',
                'indices',
                'vertices',
                'radius',
                'opacity',
                'color_scheme'
            ],
            'title': 'AddStreamlinesMessage',
            'type': 'object'
        }
    ],
    'plugin': 'DTI',
    'returns': {},
    'title': 'add-streamlines',
    'type': 'method'
}

params = {
    'name': 'test',
    'gids': [
        0,
        1,
        2
    ],
    'indices': [
        0,
        1,
        2
    ],
    'vertices': [
        0,
        1,
        2
    ],
    'opacity': 1,
    'radius': 0.1,
    'color_scheme': 0
}

result = {
    'check': 0.6984549330561658
}
