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
    'description': 'Get the current properties of the camera',
    'params': [],
    'plugin': 'Core',
    'returns': {
        'oneOf': [
            {
                'additionalProperties': False,
                'properties': {
                    'apertureRadius': {
                        'default': 0,
                        'title': 'Aperture radius',
                        'type': 'number'
                    },
                    'aspect': {
                        'default': 1,
                        'title': 'Aspect ratio',
                        'type': 'number'
                    },
                    'enableClippingPlanes': {
                        'default': True,
                        'title': 'Clipping',
                        'type': 'boolean'
                    },
                    'focusDistance': {
                        'default': 1,
                        'title': 'Focus Distance',
                        'type': 'number'
                    },
                    'fovy': {
                        'default': 45,
                        'title': 'Field of view',
                        'type': 'number'
                    }
                },
                'title': 'circuit_explorer_dof_perspective',
                'type': 'object'
            },
            {
                'additionalProperties': False,
                'properties': {
                    'apertureRadius': {
                        'default': 0,
                        'title': 'Aperture radius',
                        'type': 'number'
                    },
                    'aspect': {
                        'default': 1,
                        'title': 'Aspect ratio',
                        'type': 'number'
                    },
                    'enableClippingPlanes': {
                        'default': True,
                        'title': 'Clipping',
                        'type': 'boolean'
                    },
                    'focusDistance': {
                        'default': 1,
                        'title': 'Focus Distance',
                        'type': 'number'
                    },
                    'fovy': {
                        'default': 45,
                        'title': 'Field of view',
                        'type': 'number'
                    }
                },
                'title': 'circuit_explorer_sphere_clipping',
                'type': 'object'
            },
            {
                'additionalProperties': False,
                'properties': {
                    'apertureRadius': {
                        'default': 0,
                        'title': 'Aperture radius',
                        'type': 'number'
                    },
                    'aspect': {
                        'default': 1,
                        'readOnly': True,
                        'title': 'Aspect ratio',
                        'type': 'number'
                    },
                    'enableClippingPlanes': {
                        'default': True,
                        'title': 'Clipping',
                        'type': 'boolean'
                    },
                    'focusDistance': {
                        'default': 1,
                        'title': 'Focus Distance',
                        'type': 'number'
                    },
                    'fovy': {
                        'default': 45,
                        'title': 'Field of view',
                        'type': 'number'
                    }
                },
                'title': 'fisheye',
                'type': 'object'
            },
            {
                'additionalProperties': False,
                'properties': {
                    'aspect': {
                        'default': 1,
                        'readOnly': True,
                        'title': 'Aspect ratio',
                        'type': 'number'
                    },
                    'enableClippingPlanes': {
                        'default': True,
                        'title': 'Clipping',
                        'type': 'boolean'
                    },
                    'height': {
                        'default': 1,
                        'title': 'Height',
                        'type': 'number'
                    }
                },
                'title': 'orthographic',
                'type': 'object'
            },
            {
                'additionalProperties': False,
                'properties': {
                    'enableClippingPlanes': {
                        'default': True,
                        'title': 'Clipping',
                        'type': 'boolean'
                    },
                    'half': {
                        'default': True,
                        'title': 'Half sphere',
                        'type': 'boolean'
                    }
                },
                'title': 'panoramic',
                'type': 'object'
            },
            {
                'additionalProperties': False,
                'properties': {
                    'apertureRadius': {
                        'default': 0,
                        'title': 'Aperture radius',
                        'type': 'number'
                    },
                    'aspect': {
                        'default': 1,
                        'readOnly': True,
                        'title': 'Aspect ratio',
                        'type': 'number'
                    },
                    'enableClippingPlanes': {
                        'default': True,
                        'title': 'Clipping',
                        'type': 'boolean'
                    },
                    'focusDistance': {
                        'default': 1,
                        'title': 'Focus Distance',
                        'type': 'number'
                    },
                    'fovy': {
                        'default': 45,
                        'title': 'Field of view',
                        'type': 'number'
                    }
                },
                'title': 'perspective',
                'type': 'object'
            },
            {
                'additionalProperties': False,
                'properties': {
                    'aspect': {
                        'default': 1,
                        'readOnly': True,
                        'title': 'Aspect ratio',
                        'type': 'number'
                    },
                    'enableClippingPlanes': {
                        'default': True,
                        'title': 'Clipping',
                        'type': 'boolean'
                    },
                    'fovy': {
                        'default': 45,
                        'title': 'Field of view',
                        'type': 'number'
                    }
                },
                'title': 'perspectiveParallax',
                'type': 'object'
            }
        ]
    },
    'title': 'get-camera-params',
    'type': 'method'
}

params = None

result = {
    'check': 0.3781035371902698
}
