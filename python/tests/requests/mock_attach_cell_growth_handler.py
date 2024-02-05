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
    'description': 'Attach a dynamic cell growing rendering system for a given model',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'model_id': {
                    'description': 'The model to which to attach the handler',
                    'minimum': 0,
                    'type': 'integer'
                },
                'nb_frames': {
                    'description': 'The number of frames to perform the growth',
                    'minimum': 0,
                    'type': 'integer'
                }
            },
            'required': [
                'model_id',
                'nb_frames'
            ],
            'title': 'AttachCellGrowthHandlerMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'attach-cell-growth-handler',
    'type': 'method'
}

params = {
    'model_id': 0,
    'nb_frames': 10
}

result = {
    'check': 0.06654896648456798
}
