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
    'description': 'Retreive the material with given ID in given model',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'material_id': {
                    'description': 'Material ID',
                    'minimum': 0,
                    'type': 'integer'
                },
                'model_id': {
                    'description': 'Model ID',
                    'minimum': 0,
                    'type': 'integer'
                }
            },
            'required': [
                'model_id',
                'material_id'
            ],
            'title': 'GetMaterialMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {
        'additionalProperties': False,
        'properties': {
            'clipping_mode': {
                'description': 'The choosen material clipping mode',
                'enum': [
                    'No clipping',
                    'Plane',
                    'Sphere'
                ],
                'type': 'string'
            },
            'diffuse_color': {
                'description': 'Diffuse reflection color RGB normalized',
                'items': {
                    'type': 'number'
                },
                'maxItems': 3,
                'minItems': 3,
                'type': 'array'
            },
            'emission': {
                'description': 'The emissive property of a material',
                'type': 'number'
            },
            'glossiness': {
                'description': 'The glossy component of a material',
                'type': 'number'
            },
            'material_id': {
                'description': 'The ID that identifies this material',
                'minimum': 0,
                'type': 'integer'
            },
            'model_id': {
                'description': 'The model which this material belongs to',
                'minimum': 0,
                'type': 'integer'
            },
            'opacity': {
                'description': 'The transparency of the material (0 to 1)',
                'type': 'number'
            },
            'reflection_index': {
                'description': 'The index of reflection of the material surface',
                'type': 'number'
            },
            'refraction_index': {
                'description': 'The index of refraction of a transparent material',
                'type': 'number'
            },
            'shading_mode': {
                'description': 'The chosen shading mode',
                'enum': [
                    'None',
                    'Diffuse',
                    'Electron',
                    'Cartoon',
                    'Electron transparency',
                    'Perlin',
                    'Diffuse transparency',
                    'Checker'
                ],
                'type': 'string'
            },
            'simulation_data_cast': {
                'description': 'Wether to cast the user parameter for simulation',
                'type': 'boolean'
            },
            'specular_color': {
                'description': 'Specular reflection RGB normalized',
                'items': {
                    'type': 'number'
                },
                'maxItems': 3,
                'minItems': 3,
                'type': 'array'
            },
            'specular_exponent': {
                'description': 'The specular exponent to sharpen the specular reflection',
                'type': 'number'
            },
            'user_parameter': {
                'description': 'A custom parameter passed to the simulation',
                'type': 'number'
            }
        },
        'title': 'Material',
        'type': 'object'
    },
    'title': 'get-material',
    'type': 'method'
}

params = {
    'model_id': 1,
    'material_id': 1
}

result = {
    'check': 0.08704693067572189
}
