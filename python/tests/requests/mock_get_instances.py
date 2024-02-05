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
    'description': 'Get instances of the given model',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'id': {
                    'description': 'Model ID',
                    'minimum': 0,
                    'type': 'integer'
                },
                'result_range': {
                    'description': 'Result list from/to indices',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'maxItems': 2,
                    'minItems': 2,
                    'type': 'array'
                }
            },
            'required': [
                'id'
            ],
            'title': 'GetInstanceMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Core',
    'returns': {
        'items': {
            'additionalProperties': False,
            'properties': {
                'bounding_box': {
                    'description': 'Display bounding box',
                    'type': 'boolean'
                },
                'instance_id': {
                    'description': 'Instance ID',
                    'minimum': 0,
                    'type': 'integer'
                },
                'model_id': {
                    'description': 'Model ID',
                    'minimum': 0,
                    'type': 'integer'
                },
                'transformation': {
                    'additionalProperties': False,
                    'description': 'Model transformation',
                    'properties': {
                        'rotation': {
                            'description': 'Rotation XYZW',
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 4,
                            'minItems': 4,
                            'type': 'array'
                        },
                        'rotation_center': {
                            'description': 'Rotation center XYZ',
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 3,
                            'minItems': 3,
                            'type': 'array'
                        },
                        'scale': {
                            'description': 'Scale XYZ',
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 3,
                            'minItems': 3,
                            'type': 'array'
                        },
                        'translation': {
                            'description': 'Translation XYZ',
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 3,
                            'minItems': 3,
                            'type': 'array'
                        }
                    },
                    'title': 'Transformation',
                    'type': 'object'
                },
                'visible': {
                    'description': 'Check if rendered',
                    'type': 'boolean'
                }
            },
            'title': 'ModelInstance',
            'type': 'object'
        },
        'type': 'array'
    },
    'title': 'get-instances',
    'type': 'method'
}

params = {
    'id': 0,
    'result_range': [
        0,
        130
    ]
}

result = {
    'check': 0.19092747365498663
}
