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
    'description': 'Builds a movie file from a set of frames stored on disk',
    'params': [
        {
            'additionalProperties': False,
            'properties': {
                'dimensions': {
                    'description': 'Video dimensions (width,height)',
                    'items': {
                        'minimum': 0,
                        'type': 'integer'
                    },
                    'maxItems': 2,
                    'minItems': 2,
                    'type': 'array'
                },
                'erase_frames': {
                    'description': 'Wether to clean up the frame image files after generating the video file',
                    'type': 'boolean'
                },
                'fps_rate': {
                    'description': 'The frames per second rate at which to create the video',
                    'minimum': 0,
                    'type': 'integer'
                },
                'frames_file_extension': {
                    'description': 'The extension of the frame files to fetch (ex: png, jpg)',
                    'type': 'string'
                },
                'frames_folder_path': {
                    'description': 'Path to where to fetch the frames to create the video',
                    'type': 'string'
                },
                'output_movie_path': {
                    'description': 'The path to where the movie will be created. Must include filename and extension',
                    'type': 'string'
                }
            },
            'required': [
                'dimensions',
                'frames_folder_path',
                'frames_file_extension',
                'fps_rate',
                'output_movie_path',
                'erase_frames'
            ],
            'title': 'MakeMovieMessage',
            'type': 'object'
        }
    ],
    'plugin': 'Circuit Explorer',
    'returns': {},
    'title': 'make-movie',
    'type': 'method'
}

params = {
    'dimensions': [
        1920,
        1080
    ],
    'erase_frames': True,
    'fps_rate': 1,
    'frames_file_extension': 'png',
    'frames_folder_path': '/home/acfleury/Test/simulation',
    'output_movie_path': '/home/acfleury/Test/movie.mp4'
}

result = {
    'check': 0.3116214946867176
}
