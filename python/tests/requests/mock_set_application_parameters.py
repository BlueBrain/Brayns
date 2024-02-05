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
    'description': 'Set the current state of the application parameters',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'engine': {
                    'description': 'Application engine',
                    'readOnly': True,
                    'type': 'string'
                },
                'image_stream_fps': {
                    'description': 'Framerate of image stream',
                    'minimum': 0,
                    'type': 'integer'
                },
                'jpeg_compression': {
                    'description': 'JPEG compression rate',
                    'minimum': 0,
                    'type': 'integer'
                },
                'plugins': {
                    'description': 'Loaded plugins',
                    'items': {
                        'type': 'string'
                    },
                    'readOnly': True,
                    'type': 'array'
                },
                'viewport': {
                    'description': 'Window size',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'maxItems': 2,
                    'minItems': 2,
                    'type': 'array'
                }
            },
            'title': 'ApplicationParameters',
            'type': 'object'
        }
    ],
    'plugin': 'Core',
    'returns': {},
    'title': 'set-application-parameters',
    'type': 'method'
}

params = {
    'image_stream_fps': 90,
    'jpeg_compression': 30,
    'viewport': [
        1920,
        1080
    ]
}

result = {
    'check': 0.2821887750312474
}
