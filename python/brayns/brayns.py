#!/usr/bin/env python
# -*- coding: utf-8 -*-
# pylint: disable=C0111,R0902,R0903

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

import base64
from io import BytesIO
import json
from PIL import Image
import requests

BRAYNS_SHADING_DIFFUSE = 'diffuse'
BRAYNS_SHADING_ELECTRON = 'electron'
BRAYNS_SHADING_NONE = 'none'

BRAYNS_SHADER_BASIC = 'basic'
BRAYNS_SHADER_PROXIMITY_DETECTION = 'proximity'
BRAYNS_SHADER_SIMULATION = 'simulation'
BRAYNS_SHADER_SCIENTIFIC_VISUALIZATION = 'scientific_visualization'

# Brayns supported HTTP REST method
HTTP_METHOD_PUT = 'PUT'
HTTP_METHOD_GET = 'GET'


class HTTPWrapper(object):
    """
    Base class that wraps HTTP communication to python objects
    """
    def __init__(self, url):
        """
        :param url: Brayns' url
        """
        self._url = url

    def _serialize(self):
        """
        :return: Json representation of the object
        """
        pass

    def _request(self, method, body=None):
        """
        Queries the HTTP REST interface of Brayns for a given url and method
        :param method: PUT or GET
        :param body: Content to be sent along with the request
        :return: String containing the response from Brayns, None method is PUT of if Brayns
        could not be reached
        """
        response = None
        request = None
        try:
            if method == HTTP_METHOD_PUT:
                if body == '':
                    request = requests.put(self._url)
                else:
                    request = requests.put(self._url, data=json.dumps(body))
            elif method == HTTP_METHOD_GET:
                request = requests.get(self._url)
                response = str(request.text)
            request.close()
        except requests.exceptions.ConnectionError:
            print('ERROR: Failed to connect to Brayns, did you start it with the '\
                  '--zeroeq-http-server command line option?')
            exit(1)
        return response

    def __str__(self):
        """
        Display the serialized representation of the transfer function
        """
        return json.dumps(self._serialize())


class Camera(HTTPWrapper):

    def __init__(self, brayns_url):
        """
        :param brayns_url: Brayns' url
        """
        super(Camera, self).__init__(brayns_url + '/v1/camera')
        self._origin = [0, 0, -1]
        self._look_at = [0, 0, 0]
        self._up = [0, 1, 0]
        self._aperture = 0
        self._focal_length = 0
        ''' Initialize values from Brayns '''
        self._deserialize(self._request(HTTP_METHOD_GET))

    @property
    def origin(self):
        return self._origin

    @origin.setter
    def origin(self, origin):
        self._origin = origin
        self._request(HTTP_METHOD_PUT, self._serialize())

    @property
    def look_at(self):
        return self._look_at

    @look_at.setter
    def look_at(self, look_at):
        self._look_at = look_at

    @property
    def up_vector(self):
        return self._up

    @up_vector.setter
    def up_vector(self, up_vector):
        self._up = up_vector

    @property
    def aperture(self):
        return self._aperture

    @aperture.setter
    def aperture(self, aperture):
        self._aperture = aperture

    @property
    def focal_length(self):
        return self._focal_length

    @focal_length.setter
    def focal_length(self, focal_length):
        self._focal_length = focal_length

    def _serialize(self):
        return {
            "origin": self._origin,
            "lookAt": self._look_at,
            "up": self._up,
            "aperture": self._aperture,
            "focal_length": self._focal_length
        }

    def _deserialize(self, content):
        obj = json.loads(content)
        self._origin = obj['origin']
        self._look_at = obj['look_at']
        self._up = obj['up']
        self._aperture = obj['aperture']
        self._focal_length = obj['focal_length']


class Viewport(HTTPWrapper):

    def __init__(self, brayns_url):
        """
        :param brayns_url: Brayns' url
        """
        super(Viewport, self).__init__(brayns_url + '/v1/viewport')
        self._size = [800, 600]
        self._deserialize(self._request(HTTP_METHOD_GET))

    @property
    def size(self):
        return self._size

    @size.setter
    def size(self, size):
        self._size = size
        self._request(HTTP_METHOD_PUT, self._serialize())

    def _serialize(self):
        return {"size": self._size}

    def _deserialize(self, content):
        obj = json.loads(content)
        self._size = obj['size']


class Settings(HTTPWrapper):

    def __init__(self, brayns_url):
        """
        :param brayns_url: Brayns' url
        """
        super(Settings, self).__init__(brayns_url + '/v1/settings')
        self._ambient_occlusion = 0.0
        self._shadows = 0.0
        self._soft_shadows = 0.0
        self._jpeg_compression = 100
        self._jpeg_size = [800, 600]
        self._samples_per_pixel = 1
        self._bg_color = [0, 0, 0]
        self._epsilon = 1e-6
        self._shader = BRAYNS_SHADER_BASIC
        self._shading = BRAYNS_SHADING_DIFFUSE
        ''' Initialize values from Brayns '''
        self._deserialize(self._request(HTTP_METHOD_GET))

    @property
    def ambient_occlusion(self):
        return self._ambient_occlusion

    @ambient_occlusion.setter
    def ambient_occlusion(self, strength):
        self._ambient_occlusion = strength
        self._request(HTTP_METHOD_PUT, self._serialize())

    @property
    def jpeg_compression(self):
        return self._jpeg_compression

    @jpeg_compression.setter
    def jpeg_compression(self, compression):
        self._jpeg_compression = compression
        self._request(HTTP_METHOD_PUT, self._serialize())

    @property
    def samples_per_pixel(self):
        return self._samples_per_pixel

    @samples_per_pixel.setter
    def samples_per_pixel(self, samples_per_pixel):
        self._samples_per_pixel = samples_per_pixel
        self._request(HTTP_METHOD_PUT, self._serialize())

    @property
    def background_color(self):
        return self._bg_color

    @background_color.setter
    def background_color(self, color):
        self._bg_color = color
        self._request(HTTP_METHOD_PUT, self._serialize())

    @property
    def jpeg_size(self):
        return self._jpeg_size

    @jpeg_size.setter
    def jpeg_size(self, size):
        self._jpeg_size = size
        self._request(HTTP_METHOD_PUT, self._serialize())

    @property
    def shadows(self):
        return self._shadows

    @shadows.setter
    def shadows(self, strength):
        self._shadows = strength
        self._request(HTTP_METHOD_PUT, self._serialize())

    @property
    def soft_shadows(self):
        return self._soft_shadows

    @soft_shadows.setter
    def soft_shadows(self, strength):
        self._soft_shadows = strength
        self._request(HTTP_METHOD_PUT, self._serialize())

    @property
    def epsilon(self):
        return self._epsilon

    @epsilon.setter
    def epsilon(self, epsilon):
        self._epsilon = epsilon
        self._request(HTTP_METHOD_PUT, self._serialize())

    @property
    def shading(self):
        return self._shading

    @shading.setter
    def shading(self, shading):
        self._shading = shading
        self._request(HTTP_METHOD_PUT, self._serialize())

    @property
    def shader(self):
        return self._shader

    @shader.setter
    def shader(self, shader):
        self._shader = shader
        self._request(HTTP_METHOD_PUT, self._serialize())

    def _serialize(self):
        return {
            "background_color": self._bg_color,
            "samples_per_pixel": self._samples_per_pixel,
            "shader": self._shader,
            "shading": self._shading,
            "shadows": self._shadows,
            "soft_shadows": self._soft_shadows,
            "ambient_occlusion": self._ambient_occlusion,
            "epsilon": self._epsilon
        }

    def _deserialize(self, content):
        obj = json.loads(content)
        self._ambient_occlusion = obj["ambient_occlusion"]
        self._jpeg_quality = obj["jpeg_compression"]
        self._samples_per_pixel = obj["samples_per_pixel"]
        self._bg_color = obj["background_color"]
        self._epsilon = obj["epsilon"]
        self._shadows = obj["shadows"]
        self._soft_shadows = obj["soft_shadows"]


class ImageJPEG(HTTPWrapper):

    def __init__(self, brayns_url):
        """
        :param brayns_url: Brayns' url
        """
        super(ImageJPEG, self).__init__(brayns_url + '/v1/image-jpeg')

    def get(self):
        response = self._request(HTTP_METHOD_GET)
        if response is None:
            return None
        payload = json.loads(response)
        return Image.open(BytesIO(base64.b64decode(payload['data'])))


class FrameBuffers(HTTPWrapper):

    def __init__(self, brayns_url):
        """
        :param brayns_url: Brayns' url
        """
        super(FrameBuffers, self).__init__(brayns_url + '/v1/frame-buffers')

    def color(self):
        response = self._request(HTTP_METHOD_GET)
        if response is None:
            return None
        payload = json.loads(response)
        size = [payload['width'], payload['height']]
        return Image.frombytes('RGBA', size, base64.b64decode(payload['diffuse']))

    def depth(self):
        response = self._request(HTTP_METHOD_GET)
        if response is None:
            return None
        payload = json.loads(response)
        size = [payload['width'], payload['height']]
        return Image.frombytes('I;16', size, base64.b64decode(payload['depth']))


class Brayns(object):

    def __init__(self, url):
        """
        :param url: Brayns' url
        """
        self._url = url
        self.viewport = Viewport(url)
        self.camera = Camera(url)
        self.settings = Settings(url)

    @property
    def image_jpeg(self):
        """
        Get JPEG image from Brayns
        :return: Pillow Image object, None if image could not be retrieved
        """
        return ImageJPEG(self._url).get()

    @property
    def color_frame_buffer(self):
        """
        Get color frame buffer for Brayns
        :return: Pillow Image object, None if image could not be retrieved or frame_type is invalid
        """
        return FrameBuffers(self._url).color()

    @property
    def depth_frame_buffer(self):
        """
        Get depth frame buffer for Brayns
        :return: Pillow Image object, None if image could not be retrieved or frame_type is invalid
        """
        return FrameBuffers(self._url).depth()
