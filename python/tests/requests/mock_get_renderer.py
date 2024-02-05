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
    'description': 'Get the current state of the renderer',
    'params': [],
    'plugin': 'Core',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'accumulation': {
                'description': 'Multiple render passes',
                'type': 'boolean'
            },
            'background_color': {
                'description': 'Background color RGB',
                'items': {
                    'type': 'number'
                },
                'maxItems': 3,
                'minItems': 3,
                'type': 'array'
            },
            'current': {
                'description': 'Current renderer name',
                'type': 'string'
            },
            'head_light': {
                'description': 'Light source follows camera origin',
                'type': 'boolean'
            },
            'max_accum_frames': {
                'description': 'Max render passes',
                'minimum': 0,
                'type': 'integer'
            },
            'samples_per_pixel': {
                'description': 'Samples per pixel',
                'minimum': 0,
                'type': 'integer'
            },
            'subsampling': {
                'description': 'Subsampling',
                'minimum': 0,
                'type': 'integer'
            },
            'types': {
                'description': 'Available renderers',
                'items': {
                    'type': 'string'
                },
                'readOnly': True,
                'type': 'array'
            },
            'variance_threshold': {
                'description': 'Stop accumulation threshold',
                'type': 'number'
            }
        },
        'title': 'RenderingParameters',
        'type': 'object'
    },
    'title': 'get-renderer',
    'type': 'method'
}

params = None

result = {
    'check': 0.41771879195721207
}
