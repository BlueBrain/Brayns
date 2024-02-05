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
    'description': 'Performs neuronal tracing showing efferent and afferent synapse relationship between cells (including projections)',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'cell_gids': {
                    'description': 'List of cell GIDs to use a source of the tracing',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'type': 'array'
                },
                'connected_cells_color': {
                    'description': 'RGBA normalized color to apply to the target cells geometry',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 4,
                    'minItems': 4,
                    'type': 'array'
                },
                'model_id': {
                    'description': 'Model where to perform the neuronal tracing',
                    'minimum': 0,
                    'type': 'integer'
                },
                'non_connected_cells_color': {
                    'description': 'RGBA normalized color to apply to the rest of cells',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 4,
                    'minItems': 4,
                    'type': 'array'
                },
                'source_cell_color': {
                    'description': 'RGBA normalized color to apply to the source cell geometry',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 4,
                    'minItems': 4,
                    'type': 'array'
                },
                'target_cell_gids': {
                    'description': 'List of cells GIDs which are the result of the given tracing mode',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'type': 'array'
                }
            },
            'required': [
                'model_id',
                'cell_gids',
                'target_cell_gids',
                'source_cell_color',
                'connected_cells_color',
                'non_connected_cells_color'
            ],
            'title': 'TraceAnterogradeMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'trace-anterograde',
    'type': 'method'
}

params = {
    'model_id': 0,
    'cell_gids': [
        0,
        1,
        2
    ],
    'target_cell_gids': [
        3
    ],
    'source_cell_color': [
        0,
        0,
        1,
        1
    ],
    'connected_cells_color': [
        0,
        1,
        0,
        1
    ],
    'non_connected_cells_color': [
        1,
        0,
        0,
        1
    ]
}

result = {
    'check': 0.3714490842148158
}
