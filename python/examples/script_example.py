#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2017, Blue Brain Project
#                          Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns
# <https://github.com/BlueBrain/Brayns>
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
# All rights reserved. Do not distribute without further notice.

from brayns.brayns import *

# --------------------------------------------------
# Initialize brayns with Brayns' url
# --------------------------------------------------
brayns = Brayns('http://localhost:5000')

# --------------------------------------------------
# Activate default viewport
# --------------------------------------------------
viewport = brayns.viewport
viewport.size = [512, 256]

# --------------------------------------------------
# Define and set camera defined by origin, look-at,
# up vector, aperture and focal length
# --------------------------------------------------
camera = brayns.camera
camera.origin = [0.5, 0.5, 2.0]
camera.look_at = [0.5, 0.5, 0.5]
camera.up_vector = [0, 1, 0]

# --------------------------------------------------
# Rendering settings
# --------------------------------------------------
settings = brayns.settings

# Set background color
settings.background_color = [0.1, 0.1, 0.5]
# set source image size
settings.window_size = [512, 512]
# Set number of samples per pixel
settings.samples_per_pixel = 1
# Activate basic shader
settings.shader = BRAYNS_SHADER_SCIENTIFIC_VISUALIZATION
# set ambient occlusion strength
settings.ambient_occlusion = 1

# Activate shadows and make them soft
settings.shadows = 0.5
settings.soft_shadows = 0.02

# Epsilon
settings.epsilon = 0.001

# --------------------------------------------------
# Get JPEG image back and save it to example.jpg
# --------------------------------------------------
settings.jpeg_size = [512, 512]
settings.jpeg_compression = 100
image = brayns.image_jpeg
if image is not None:
    image.save('example.jpg')

# --------------------------------------------------
# Get frame buffers (Color and Depth)
# --------------------------------------------------
image = brayns.color_frame_buffer
if image is not None:
    image.save('fb_color.tif')
image = brayns.depth_frame_buffer
if image is not None:
    image.save('fb_depth.tif')
