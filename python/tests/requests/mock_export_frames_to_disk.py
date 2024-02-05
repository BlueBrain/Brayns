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
    'description': 'Export a set of frames from a simulation as image files',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'animation_information': {
                    'description': 'A list of frame numbers to render',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'type': 'array'
                },
                'camera_information': {
                    'description': 'A list of camera definitions. Each camera definition contains origin, direction, up, apperture and radius. (1 entry per animation information entry)',
                    'items': {
                        'type': 'number'
                    },
                    'type': 'array'
                },
                'format': {
                    'description': 'The image format (PNG or JPEG)',
                    'type': 'string'
                },
                'name_after_step': {
                    'description': 'Name the file on disk after the simulation step index',
                    'type': 'boolean'
                },
                'path': {
                    'description': 'Directory to store the frames',
                    'type': 'string'
                },
                'quality': {
                    'description': 'The quality at which the images will be stored',
                    'minimum': 0,
                    'type': 'integer'
                },
                'spp': {
                    'description': 'Samples per pixels',
                    'minimum': 0,
                    'type': 'integer'
                },
                'start_frame': {
                    'description': 'The frame at which to start exporting frames',
                    'minimum': 0,
                    'type': 'integer'
                }
            },
            'required': [
                'path',
                'format',
                'name_after_step',
                'quality',
                'spp',
                'start_frame',
                'animation_information',
                'camera_information'
            ],
            'title': 'ExportFramesToDisk',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'export-frames-to-disk',
    'type': 'method'
}

params = {
    'path': '/home/acfleury/Test/simulation',
    'format': 'png',
    'name_after_step': False,
    'quality': 100,
    'spp': 1,
    'start_frame': 0,
    'animation_information': [
        0,
        1,
        2,
        3
    ],
    'camera_information': [
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        2,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        2,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        2,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        2
    ]
}

result = {
    'check': 0.5989378328182542
}
