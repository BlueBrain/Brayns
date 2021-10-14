# Copyright (c) 2015-2021 EPFL/Blue Brain Project
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
    'description': 'Set the current properties of the renderer',
    'params': [
        {
            'oneOf': [
                {
                    'title': 'basic',
                    'type': 'object'
                },
                {
                    'additionalProperties': False,
                    'properties': {
                        'epsilonFactor': {
                            'default': 1,
                            'title': 'Epsilon factor',
                            'type': 'number'
                        },
                        'exposure': {
                            'default': 1,
                            'title': 'Exposure',
                            'type': 'number'
                        },
                        'fogStart': {
                            'default': 0,
                            'title': 'Fog start',
                            'type': 'number'
                        },
                        'fogThickness': {
                            'default': 1000000,
                            'title': 'Fog thickness',
                            'type': 'number'
                        },
                        'giDistance': {
                            'default': 10000,
                            'title': 'Global illumination distance',
                            'type': 'number'
                        },
                        'giSamples': {
                            'default': 0,
                            'title': 'Global illumination samples',
                            'type': 'integer'
                        },
                        'giWeight': {
                            'default': 0,
                            'title': 'Global illumination weight',
                            'type': 'number'
                        },
                        'maxBounces': {
                            'default': 3,
                            'title': 'Maximum number of ray bounces',
                            'type': 'integer'
                        },
                        'maxDistanceToSecondaryModel': {
                            'default': 30,
                            'title': 'Maximum distance to secondary model',
                            'type': 'number'
                        },
                        'samplingThreshold': {
                            'default': 0.001,
                            'title': 'Threshold under which sampling is ignored',
                            'type': 'number'
                        },
                        'shadows': {
                            'default': 0,
                            'title': 'Shadow intensity',
                            'type': 'number'
                        },
                        'softShadows': {
                            'default': 0,
                            'title': 'Shadow softness',
                            'type': 'number'
                        },
                        'softShadowsSamples': {
                            'default': 1,
                            'title': 'Soft shadow samples',
                            'type': 'integer'
                        },
                        'useHardwareRandomizer': {
                            'default': False,
                            'title': 'Use hardware accelerated randomizer',
                            'type': 'boolean'
                        },
                        'volumeAlphaCorrection': {
                            'default': 0.5,
                            'title': 'Volume alpha correction',
                            'type': 'number'
                        },
                        'volumeSpecularExponent': {
                            'default': 20,
                            'title': 'Volume specular exponent',
                            'type': 'number'
                        }
                    },
                    'title': 'circuit_explorer_advanced',
                    'type': 'object'
                },
                {
                    'additionalProperties': False,
                    'properties': {
                        'alphaCorrection': {
                            'default': 0.5,
                            'title': 'Alpha correction',
                            'type': 'number'
                        },
                        'exposure': {
                            'default': 1,
                            'title': 'Exposure',
                            'type': 'number'
                        },
                        'maxBounces': {
                            'default': 3,
                            'title': 'Maximum number of ray bounces',
                            'type': 'integer'
                        },
                        'maxDistanceToSecondaryModel': {
                            'default': 30,
                            'title': 'Maximum distance to secondary model',
                            'type': 'number'
                        },
                        'simulationThreshold': {
                            'default': 0,
                            'title': 'Simulation threshold',
                            'type': 'number'
                        },
                        'useHardwareRandomizer': {
                            'default': False,
                            'title': 'Use hardware accelerated randomizer',
                            'type': 'boolean'
                        }
                    },
                    'title': 'circuit_explorer_basic',
                    'type': 'object'
                },
                {
                    'additionalProperties': False,
                    'properties': {
                        'alphaCorrection': {
                            'default': 0.5,
                            'title': 'Alpha correction',
                            'type': 'number'
                        },
                        'exposure': {
                            'default': 1,
                            'title': 'Exposure',
                            'type': 'number'
                        },
                        'fogStart': {
                            'default': 0,
                            'title': 'Fog start',
                            'type': 'number'
                        },
                        'fogThickness': {
                            'default': 1000000,
                            'title': 'Fog thickness',
                            'type': 'number'
                        },
                        'shadowDistance': {
                            'default': 10000,
                            'title': 'Shadow distance',
                            'type': 'number'
                        },
                        'shadows': {
                            'default': 0,
                            'title': 'Shadow intensity',
                            'type': 'number'
                        },
                        'simulationThreshold': {
                            'default': 0,
                            'title': 'Simulation threshold',
                            'type': 'number'
                        },
                        'softShadows': {
                            'default': 0,
                            'title': 'Shadow softness',
                            'type': 'number'
                        },
                        'tfColor': {
                            'default': False,
                            'title': 'Use transfer function color',
                            'type': 'boolean'
                        },
                        'useHardwareRandomizer': {
                            'default': False,
                            'title': 'Use hardware accelerated randomizer',
                            'type': 'boolean'
                        }
                    },
                    'title': 'circuit_explorer_cell_growth',
                    'type': 'object'
                },
                {
                    'additionalProperties': False,
                    'properties': {
                        'alphaCorrection': {
                            'default': 0.5,
                            'title': 'Alpha correction',
                            'type': 'number'
                        },
                        'detectionDistance': {
                            'default': 1,
                            'title': 'Detection distance',
                            'type': 'number'
                        },
                        'detectionFarColor': {
                            'default': [
                                1,
                                0,
                                0
                            ],
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 3,
                            'minItems': 3,
                            'title': 'Detection far color',
                            'type': 'array'
                        },
                        'detectionNearColor': {
                            'default': [
                                0,
                                1,
                                0
                            ],
                            'items': {
                                'type': 'number'
                            },
                            'maxItems': 3,
                            'minItems': 3,
                            'title': 'Detection near color',
                            'type': 'array'
                        },
                        'detectionOnDifferentMaterial': {
                            'default': False,
                            'title': 'Detection on different material',
                            'type': 'boolean'
                        },
                        'exposure': {
                            'default': 1,
                            'title': 'Exposure',
                            'type': 'number'
                        },
                        'maxBounces': {
                            'default': 3,
                            'title': 'Maximum number of ray bounces',
                            'type': 'integer'
                        },
                        'surfaceShadingEnabled': {
                            'default': True,
                            'title': 'Surface shading',
                            'type': 'boolean'
                        },
                        'useHardwareRandomizer': {
                            'default': False,
                            'title': 'Use hardware accelerated randomizer',
                            'type': 'boolean'
                        }
                    },
                    'title': 'circuit_explorer_proximity_detection',
                    'type': 'object'
                },
                {
                    'additionalProperties': False,
                    'properties': {
                        'alphaCorrection': {
                            'default': 0.5,
                            'title': 'Alpha correction',
                            'type': 'number'
                        },
                        'exposure': {
                            'default': 1,
                            'title': 'Exposure',
                            'type': 'number'
                        },
                        'fogStart': {
                            'default': 0,
                            'title': 'Fog start',
                            'type': 'number'
                        },
                        'fogThickness': {
                            'default': 1000000,
                            'title': 'Fog thickness',
                            'type': 'number'
                        },
                        'maxBounces': {
                            'default': 3,
                            'title': 'Maximum number of ray bounces',
                            'type': 'integer'
                        },
                        'simulationThreshold': {
                            'default': 0,
                            'title': 'Simulation threshold',
                            'type': 'number'
                        },
                        'useHardwareRandomizer': {
                            'default': False,
                            'title': 'Use hardware accelerated randomizer',
                            'type': 'boolean'
                        }
                    },
                    'title': 'circuit_explorer_voxelized_simulation',
                    'type': 'object'
                },
                {
                    'additionalProperties': False,
                    'properties': {
                        'maxContribution': {
                            'default': 100000,
                            'title': 'Max contribution',
                            'type': 'number'
                        },
                        'rouletteDepth': {
                            'default': 5,
                            'title': 'Roulette depth',
                            'type': 'integer'
                        }
                    },
                    'title': 'pathtracer',
                    'type': 'object'
                },
                {
                    'title': 'raycast_Ng',
                    'type': 'object'
                },
                {
                    'title': 'raycast_Ns',
                    'type': 'object'
                },
                {
                    'additionalProperties': False,
                    'properties': {
                        'aoDistance': {
                            'default': 10000,
                            'title': 'Ambient occlusion distance',
                            'type': 'number'
                        },
                        'aoSamples': {
                            'default': 1,
                            'title': 'Ambient occlusion samples',
                            'type': 'integer'
                        },
                        'aoTransparencyEnabled': {
                            'default': True,
                            'title': 'Ambient occlusion transparency',
                            'type': 'boolean'
                        },
                        'aoWeight': {
                            'default': 0,
                            'title': 'Ambient occlusion weight',
                            'type': 'number'
                        },
                        'oneSidedLighting': {
                            'default': True,
                            'title': 'One-sided lighting',
                            'type': 'boolean'
                        },
                        'shadowsEnabled': {
                            'default': False,
                            'title': 'Shadows',
                            'type': 'boolean'
                        }
                    },
                    'title': 'scivis',
                    'type': 'object'
                }
            ]
        }
    ],
    'plugin': 'Core',
    'returns': {},
    'title': 'set-renderer-params',
    'type': 'method'
}

params = {
    'volumeSpecularExponent': 22,
    'volumeAlphaCorrection': 0.6,
    'useHardwareRandomizer': True,
    'softShadowsSamples': 2
}

result = {
    'check': 0.747378084769601
}
