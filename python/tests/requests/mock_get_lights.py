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
    'description': 'Get all lights',
    'params': [],
    'plugin': 'Core',
    'returns': {
        'items': {
            'additionalProperties': False,
            'properties': {
                'id': {
                    'description': 'Light ID',
                    'minimum': 0,
                    'type': 'integer'
                },
                'properties': {
                    'description': 'Light properties',
                    'oneOf': [
                        {
                            'additionalProperties': False,
                            'properties': {
                                'angular_diameter': {
                                    'description': 'Angular diameter in degrees',
                                    'type': 'number'
                                },
                                'color': {
                                    'description': 'Light color RGB',
                                    'items': {
                                        'type': 'number'
                                    },
                                    'maxItems': 3,
                                    'minItems': 3,
                                    'type': 'array'
                                },
                                'direction': {
                                    'description': 'Light source direction',
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
                                'is_visible',
                                'direction',
                                'angular_diameter'
                            ],
                            'title': 'directional',
                            'type': 'object'
                        },
                        {
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
                                },
                                'position': {
                                    'description': 'Light position XYZ',
                                    'items': {
                                        'type': 'number'
                                    },
                                    'maxItems': 3,
                                    'minItems': 3,
                                    'type': 'array'
                                },
                                'radius': {
                                    'description': 'Sphere radius',
                                    'type': 'number'
                                }
                            },
                            'required': [
                                'color',
                                'intensity',
                                'is_visible',
                                'position',
                                'radius'
                            ],
                            'title': 'sphere',
                            'type': 'object'
                        },
                        {
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
                                'edge1': {
                                    'description': 'First edge XYZ',
                                    'items': {
                                        'type': 'number'
                                    },
                                    'maxItems': 3,
                                    'minItems': 3,
                                    'type': 'array'
                                },
                                'edge2': {
                                    'description': 'Second edge XYZ',
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
                                },
                                'position': {
                                    'description': 'Light position XYZ',
                                    'items': {
                                        'type': 'number'
                                    },
                                    'maxItems': 3,
                                    'minItems': 3,
                                    'type': 'array'
                                }
                            },
                            'required': [
                                'color',
                                'intensity',
                                'is_visible',
                                'position',
                                'edge1',
                                'edge2'
                            ],
                            'title': 'quad',
                            'type': 'object'
                        },
                        {
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
                                'direction': {
                                    'description': 'Spot direction XYZ',
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
                                },
                                'opening_angle': {
                                    'description': 'Opening angle in degrees',
                                    'type': 'number'
                                },
                                'penumbra_angle': {
                                    'description': 'Penumbra angle in degrees',
                                    'type': 'number'
                                },
                                'position': {
                                    'description': 'Light position XYZ',
                                    'items': {
                                        'type': 'number'
                                    },
                                    'maxItems': 3,
                                    'minItems': 3,
                                    'type': 'array'
                                },
                                'radius': {
                                    'description': 'Spot radius',
                                    'type': 'number'
                                }
                            },
                            'required': [
                                'color',
                                'intensity',
                                'is_visible',
                                'position',
                                'direction',
                                'opening_angle',
                                'penumbra_angle',
                                'radius'
                            ],
                            'title': 'spotlight',
                            'type': 'object'
                        },
                        {
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
                            'title': 'ambient',
                            'type': 'object'
                        }
                    ],
                    'title': 'LightProperties'
                },
                'type': {
                    'description': 'Light type',
                    'enum': [
                        'sphere',
                        'directional',
                        'quad',
                        'spotlight',
                        'ambient'
                    ],
                    'type': 'string'
                }
            },
            'required': [
                'type',
                'id',
                'properties'
            ],
            'title': 'LightMessage',
            'type': 'object'
        },
        'type': 'array'
    },
    'title': 'get-lights',
    'type': 'method'
}

params = None

result = {
    'check': 0.21434972141490327
}
