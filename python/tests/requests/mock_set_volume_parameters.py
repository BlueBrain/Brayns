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
    'description': 'Set the current state of the volume parameters',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'adaptive_max_sampling_rate': {
                    'description': 'Max sampling rate',
                    'type': 'number'
                },
                'adaptive_sampling': {
                    'description': 'Use adaptive sampling',
                    'type': 'boolean'
                },
                'clip_box': {
                    'additionalProperties': False,
                    'description': 'Clip box',
                    'properties': {
                        'max': {
                            'description': 'Top-right XYZ',
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 3,
                            'minItems': 3,
                            'type': 'array'
                        },
                        'min': {
                            'description': 'Bottom-left XYZ',
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 3,
                            'minItems': 3,
                            'type': 'array'
                        }
                    },
                    'title': 'Boxd',
                    'type': 'object'
                },
                'gradient_shading': {
                    'description': 'Use gradient shading',
                    'type': 'boolean'
                },
                'pre_integration': {
                    'description': 'Use pre-integration',
                    'type': 'boolean'
                },
                'sampling_rate': {
                    'description': 'Fixed sampling rate',
                    'type': 'number'
                },
                'single_shade': {
                    'description': 'Use a single shade for the whole volume',
                    'type': 'boolean'
                },
                'specular': {
                    'description': 'Reflectivity amount XYZ',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'volume_dimensions': {
                    'description': 'Dimensions XYZ',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'volume_element_spacing': {
                    'description': 'Element spacing XYZ',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                },
                'volume_offset': {
                    'description': 'Offset XYZ',
                    'items': {
                        'type': 'number'
                    },
                    'maxItems': 3,
                    'minItems': 3,
                    'type': 'array'
                }
            },
            'title': 'VolumeParameters',
            'type': 'object'
        }
    ],
    'plugin': 'Core',
    'returns': {},
    'title': 'set-volume-parameters',
    'type': 'method'
}

params = {
    'volume_dimensions': [
        1,
        1,
        1
    ],
    'volume_element_spacing': [
        0.1,
        0.1,
        0.1
    ],
    'volume_offset': [
        1,
        1,
        1
    ],
    'gradient_shading': True,
    'single_shade': False,
    'pre_integration': True,
    'adaptive_max_sampling_rate': 3,
    'adaptive_sampling': False,
    'sampling_rate': 0.2,
    'specular': [
        1,
        1,
        1
    ],
    'clip_box': {
        'min': [
            0,
            0,
            0
        ],
        'max': [
            1,
            1,
            1
        ]
    }
}

result = {
    'check': 0.7624704446264586
}
