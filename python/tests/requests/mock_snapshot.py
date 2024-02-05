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
    'async': True,
    'description': 'Take a snapshot with given parameters',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'animation_parameters': {
                    'additionalProperties': False,
                    'description': 'Animation parameters',
                    'properties': {
                        'current': {
                            'description': 'Current frame index',
                            'minimum': 0,
                            'type': 'integer'
                        },
                        'delta': {
                            'description': 'Frame delta',
                            'type': 'integer'
                        },
                        'dt': {
                            'description': 'Frame time',
                            'type': 'number'
                        },
                        'frame_count': {
                            'description': 'Animation frame count',
                            'minimum': 0,
                            'type': 'integer'
                        },
                        'playing': {
                            'description': 'Animation is playing',
                            'type': 'boolean'
                        },
                        'unit': {
                            'description': 'Time unit',
                            'type': 'string'
                        }
                    },
                    'title': 'AnimationParameters',
                    'type': 'object'
                },
                'camera': {
                    'additionalProperties': False,
                    'description': 'Camera parameters',
                    'properties': {
                        'current': {
                            'description': 'Camera current type',
                            'type': 'string'
                        },
                        'orientation': {
                            'description': 'Camera orientation XYZW',
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 4,
                            'minItems': 4,
                            'type': 'array'
                        },
                        'position': {
                            'description': 'Camera position XYZ',
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 3,
                            'minItems': 3,
                            'type': 'array'
                        },
                        'target': {
                            'description': 'Camera target XYZ',
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 3,
                            'minItems': 3,
                            'type': 'array'
                        },
                        'types': {
                            'description': 'Available camera types',
                            'items': {
                                'type': 'string'
                            },
                            'readOnly': True,
                            'type': 'array'
                        }
                    },
                    'title': 'Camera',
                    'type': 'object'
                },
                'file_path': {
                    'description': 'Path if saved on disk',
                    'type': 'string'
                },
                'format': {
                    'description': 'Image format (extension without dot)',
                    'type': 'string'
                },
                'name': {
                    'description': 'Name of the snapshot',
                    'type': 'string'
                },
                'quality': {
                    'description': 'Image quality from 0 to 100',
                    'minimum': 0,
                    'type': 'integer'
                },
                'renderer': {
                    'additionalProperties': False,
                    'description': 'Renderer parameters',
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
                'samples_per_pixel': {
                    'description': 'Samples per pixel',
                    'type': 'integer'
                },
                'size': {
                    'description': 'Image dimensions',
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
                'format',
                'size'
            ],
            'title': 'SnapshotParams',
            'type': 'object'
        }
    ],
    'plugin': 'Core',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'data': {
                'description': 'Image data with base64 encoding',
                'type': 'string'
            }
        },
        'required': [
            'data'
        ],
        'title': 'ImageBase64',
        'type': 'object'
    },
    'title': 'snapshot',
    'type': 'method'
}

params = {
    'size': [
        1920,
        1080
    ],
    'format': 'jpg',
    'samples_per_pixel': 128
}

result = {
    'check': 0.3950221859848323
}
