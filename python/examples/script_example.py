#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016, Blue Brain Project
#                     Cyrille Favreau <cyrille.favreau@epfl.ch>
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
# Activate default renderer
# --------------------------------------------------
brayns.renderer = BRAYNS_RENDERER_DEFAULT

# --------------------------------------------------
# Activate no shading shader
# --------------------------------------------------
brayns.shader = BRAYNS_SHADER_DIFFUSE

# --------------------------------------------------
# set ambient occlusion strength
# --------------------------------------------------
brayns.ambient_occlusion = 1.0

# --------------------------------------------------
# Activate shadows and make them soft
# --------------------------------------------------
brayns.shadows = True
brayns.soft_shadows = True

# --------------------------------------------------
# Define and set camera defined by origin, look-at,
# up vector, aperture and focal length
# --------------------------------------------------
camera = Camera()
camera.origin = [0.0,0.0,-3.0]
camera.look_at = [0.0,0.0,0.0]
brayns.camera = camera

# --------------------------------------------------
# Set material 0 to white
# --------------------------------------------------
material = Material()
material.index = 0
material.diffuse_color = [1.0,1.0,1.0]
material.specular_color= [1.0,1.0,1.0]
material.specular_exponent = 100.0
material.opacity = 1.0
brayns.material = material

# --------------------------------------------------
# Set background color
# --------------------------------------------------
brayns.background_color = [0.1, 0.1, 0.1]

# --------------------------------------------------
# set source image size
# --------------------------------------------------
brayns.window_size = [512, 512]

# --------------------------------------------------
# Set number of samples per pixel
# --------------------------------------------------
brayns.samples_per_pixel = 1

# --------------------------------------------------
# Define transfer function for electrical simulation
# --------------------------------------------------
transfer_function = TransferFunction()

# Define control points for all attributes (R,G,B,A)
red_control_points = [
    [ -92.0915, 0.1 ], [-61.0, 0.1 ],
    [-50.0, 0.8 ], [0.0, 0.0], [49.5497, 1]]
transfer_function.set_control_points(
    BRAYNS_ATTRIBUTE_RED, red_control_points)

green_control_points = [
    [ -92.0915, 0.1 ], [-55.0, 0.1 ],
    [-50.0, 0.5 ], [49.5497, 1]]
transfer_function.set_control_points(
    BRAYNS_ATTRIBUTE_GREEN, green_control_points)

blue_control_points = [
    [ -92.0915, 0.1 ], [-50.0, 0.1 ],
    [-58.0, 0.0 ], [0.0, 0.1]]
transfer_function.set_control_points(
    BRAYNS_ATTRIBUTE_BLUE, blue_control_points)

alpha_control_points = [
    [ -92.0915, 1.0 ], [49.5497, 1]]
transfer_function.set_control_points(
    BRAYNS_ATTRIBUTE_ALPHA, alpha_control_points)

# Set transfer function
brayns.transfer_function = transfer_function

# --------------------------------------------------
# Get JPEG image back and save it to example.jpg
# --------------------------------------------------
brayns.image_jpeg_size = [512, 512]
brayns.image_jpeg_quality = 100
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
