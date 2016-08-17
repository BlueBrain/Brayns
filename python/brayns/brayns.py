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

import requests
import json
from PIL import Image
from io import BytesIO
import base64

BRAYNS_ATTRIBUTE_RED = 'red'
BRAYNS_ATTRIBUTE_GREEN = 'green'
BRAYNS_ATTRIBUTE_BLUE = 'blue'
BRAYNS_ATTRIBUTE_ALPHA = 'alpha'
BRAYNS_ATTRIBUTE_LIGHT_EMISSION = 'emission'

BRAYNS_SHADER_DIFFUSE = 'diffuse'
BRAYNS_SHADER_ELECTRON = 'electron'
BRAYNS_SHADER_NOSHADING = 'noshading'

BRAYNS_FRAMEBUFFER_COLOR = 0
BRAYNS_FRAMEBUFFER_DEPTH = 1

BRAYNS_RENDERER_DEFAULT = 'exobj'
BRAYNS_RENDERER_PROXIMITY_DETECTION = 'proximityrenderer'
BRAYNS_RENDERER_SIMULATION = 'simulationrenderer'

# Brayns supported HTTP REST method
HTTP_METHOD_PUT = 'PUT'
HTTP_METHOD_GET = 'GET'

# Brayns supported attributes
BRAYNS_ATTRIBUTE_AMBIENT_OCCLUSION = 'ambient-occlusion'
BRAYNS_ATTRIBUTE_SHADOWS = 'shadows'
BRAYNS_ATTRIBUTE_SOFT_SHADOWS = 'soft-shadows'
BRAYNS_ATTRIBUTE_EPSILON = 'epsilon'
BRAYNS_ATTRIBUTE_SAMPLES_PER_PIXEL = 'spp'
BRAYNS_ATTRIBUTE_BACKGROUND_COLOR = 'background-color'
BRAYNS_ATTRIBUTE_WINDOW_SIZE = 'window-size'
BRAYNS_ATTRIBUTE_JPEG_SIZE = 'jpeg-size'
BRAYNS_ATTRIBUTE_JPEG_COMPRESSION = 'jpeg-compression'
BRAYNS_ATTRIBUTE_TIMESTAMP = 'timestamp'
BRAYNS_ATTRIBUTE_MATERIAL = 'material'
BRAYNS_ATTRIBUTE_RENDERER = 'renderer'


class Camera(object):
    """
    The camera object is defined by an origin, a look at position,
    an up vector, a aperture and a focal length
    """

    def __init__(self):
        self._origin = [0,0,-1]
        self._look_at = [0,0,0]
        self._up = [0,1,0]
        self._aperture = 0
        self._focal_length = 0

    @property
    def origin(self):
        return self._origin

    @origin.setter
    def origin(self, origin):
        """
        Set for origin position of the camera
        :param origin: X, Y and Z coordinates
        """
        self._origin = origin

    @property
    def look_at(self):
        return self._look_at

    @look_at.setter
    def look_at(self, look_at):
        """
        Set for look at position of the camera
        :param look_at: X, Y and Z coordinates
        """
        self._look_at = look_at

    @property
    def up(self):
        return self._up

    @up.setter
    def up(self, up):
        """
        Set for up vector of the camera
        :param up: X, Y and Z coordinates
        """
        self._up = up

    @property
    def aperture(self):
        return self._aperture

    @aperture.setter
    def aperture(self, aperture):
        """
        Set the aperture of the camera
        :param aperture: Aperture
        """
        self._aperture = aperture

    @property
    def focal_length(self):
        return self._focal_length

    @focal_length.setter
    def focal_length(self, focal_length):
        """
        Set the focal length aperture
        :param focal_length: Focal length
        """
        self._focal_length = focal_length

    def serialize(self):
        """
        Serialize camera object into as JSON string
        :return : String containing a JSON representation of the camera
        """
        payload = {
            "origin": {
                "x": self._origin[0],
                "y": self._origin[1],
                "z": self._origin[2]
            },
            "lookAt": {
                "x": self._look_at[0],
                "y": self._look_at[1],
                "z": self._look_at[2]
            },
            "up": {
                "x": self._up[0],
                "y": self._up[1],
                "z": self._up[2]
            },
            "fovAperture": self._aperture,
            "fovFocalLength": self._focal_length
        }
        return json.dumps(payload)

    def deserialize(self, content):
        """
        Deserialize camera object from JSON string
        :param : content: String containing a JSON representation of the camera
        """
        obj = json.loads(content)
        self._origin[0] = obj['origin']['x']
        self._origin[1] = obj['origin']['y']
        self._origin[2] = obj['origin']['z']
        self._look_at[0] = obj['lookAt']['x']
        self._look_at[1] = obj['lookAt']['y']
        self._look_at[2] = obj['lookAt']['z']
        self._up[0] = obj['up']['x']
        self._up[1] = obj['up']['y']
        self._up[2] = obj['up']['z']
        self._aperture = obj['fovAperture']
        self._focal_length = obj['fovFocalLength']

    def __str__(self):
        """
        Display the serialized representation of the camera
        """
        return self.serialize()


class Material(object):
    """
    The material objet is defined by a diffuse color, a specular color, a specular
    component, an opacity, a refraction index, a reflection index and a light
    emission intensity. Brayns holds a list of materials (200 by default)
    """

    def __init__(self):
        self._index = 0
        self._diffuse_color = [0.0, 0.0, 0.0]
        self._specular_color = [0.0, 0.0, 0.0]
        self._specular_exponent = 0.0
        self._opacity = 0.0
        self._refraction_index = 0.0
        self._reflection_index = 0.0
        self._light_emission = 0.0

    @property
    def index(self):
        return self._index

    @index.setter
    def index(self, index):
        """
        Set the index for the current material
        :param index: Material index
        """
        self._index = index

    @property
    def diffuse_color(self):
        return self._diffuse_color

    @diffuse_color.setter
    def diffuse_color(self, color):
        """
        Set the diffuse color for the material
        :param color: Red, Green and Blue components [0..1]
        """
        self._diffuse_color = color

    @property
    def specular_color(self):
        return self._specular_color

    @specular_color.setter
    def specular_color(self, color):
        """
        Set the specular color for the material
        :param color: Red, Green and Blue components [0..1]
        """
        self._specular_color = color

    @property
    def specular_exponent(self):
        return self._specular_exponent

    @specular_exponent.setter
    def specular_exponent(self, exponent):
        """
        Set the specular exponent for the material
        :param exponent: Specular exponent [0..1]
        """
        self._specular_exponent = exponent

    @property
    def opacity(self):
        return self._opacity

    @opacity.setter
    def opacity(self, opacity):
        """
        Set the opacity for the material. 0 is transparent, 1 is opaque
        :param opacity: Opacity [0..1]
        """
        self._opacity = opacity

    @property
    def refraction_index(self):
        return self._refraction_index

    @refraction_index.setter
    def refraction_index(self, refraction_index):
        """
        Set the refraction index for the material. This attribute is ignored by opaque materials
        :param refraction_index: Refraction index
        """
        self._refraction_index = refraction_index

    @property
    def reflection_index(self):
        return self._reflection_index

    @reflection_index.setter
    def reflection_index(self, reflection_index):
        """
        Set the reflection index for the material. 0 for no reflection, 1 for full reflection
        :param reflection_index: Reflection index [0..1]
        """
        self._reflection_index = reflection_index

    @property
    def light_emission(self):
        return self._light_emission

    @light_emission.setter
    def light_emission(self, light_emission):
        """
        Set the light emission intensity for the material. 0 for no emission, 1 for full intensity
        :param light_emission: Light emission intensity [0..1]
        """
        self._light_emission = light_emission

    def serialize(self):
        """
        Serialize material object into as JSON string
        :return : String containing a JSON representation of the material
        """
        payload = {
            "index": self._index,
            "diffuseColor" :
            {
                "r" : self._diffuse_color[0],
                "g" : self._diffuse_color[1],
                "b" : self._diffuse_color[2]
            },
            "lightEmission" : self._light_emission,
            "opacity" : self._opacity,
            "reflectionIndex" : self._reflection_index,
            "refractionIndex" : self._refraction_index,
            "specularColor" :
            {
                "r" : self._specular_color[0],
                "g" : self._specular_color[1],
                "b" : self._specular_color[2]
            },
            "specularExponent" : self._specular_exponent
        }
        return json.dumps(payload)

    def deserialize(self, content):
        """
        Deserialize material object from JSON string
        :param : content: String containing a JSON representation of the material
        """
        obj = json.loads(content)
        self._index = obj['index']
        self._diffuse_color[0] = obj['diffuseColor']['r']
        self._diffuse_color[1] = obj['diffuseColor']['g']
        self._diffuse_color[2] = obj['diffuseColor']['b']
        self._specular_color[0] = obj['specularColor']['r']
        self._specular_color[1] = obj['specularColor']['g']
        self._specular_color[2] = obj['specularColor']['b']
        self._specular_exponent = obj['specularExponent']
        self._opacity = obj['opacity']
        self._refraction_index = obj['refractionIndex']
        self._reflection_index = obj['reflectionIndex']
        self._light_emission = obj['lightEmission']

    def __str__(self):
        """
        Display the serialized representation of the material
        """
        return self.serialize()


class TransferFunction(object):
    """
    The transfer function object holds a list of control points for various attributes such as the red, green, blue and
    alpha component. In the context of Brayns, the transfer function also holds control points for the intensity of
    light emitted by materials
    """

    def __init__(self):
        """
        Initialized the transfer function object
        :param : session: Session holding the rendering resource
        :param : request: HTTP request with a body containing a JSON representation of the process
                 parameters
        :rtype : An HTTP response containing the status and description of the command
        """
        self._control_points = dict()
        self._control_points[BRAYNS_ATTRIBUTE_RED] = []
        self._control_points[BRAYNS_ATTRIBUTE_GREEN] = []
        self._control_points[BRAYNS_ATTRIBUTE_BLUE] = []
        self._control_points[BRAYNS_ATTRIBUTE_ALPHA] = []

    def get_attributes(self):
        """
        Get transfer function attributes such as R, G, B or Alpha
        :rtype : A dictionnary of attributes
        """
        attributes = []
        for attribute in self._control_points:
            attributes.append(attribute)
        return attributes

    def get_control_points(self, attribute):
        """
        Get control points for a given attribute
        :param : attribute: Attribute such as R, G, B or A
        :rtype : A dictionnay of control points
        """
        return self._control_points[attribute]

    def set_control_points(self, attribute, control_points):
        """
        Set control points to a given attribute. Existing control points are removed and replaced.
        :param : attribute: Attribute such as R, G, B or A
        :param : control_points Control points for the given attribute
        """
        del self._control_points[attribute]
        self._control_points[attribute] = []
        for control_point in control_points:
            self._control_points[attribute].append( control_point )

    def serialize(self, attribute):
        """
        Serialize the transfer function object into as JSON string
        :return : A string containing a JSON representation of the transfer function
        """
        payload = '{"attribute": "' + attribute + '", "points": ['
        count = 0
        for point in self._control_points[attribute]:
            if count != 0:
                payload = payload + ','
            payload = payload + '{"x":' + str(point[0]) + ',"y":' + str(point[1]) + '}'
            count += 1
        payload = payload + ']}'
        return payload

    def deserialize(self, content):
        """
        Deserialize transfert function object from JSON string
        :param : content: String containing a JSON representation of the material
        """
        obj = json.loads(content)
        attribute = obj['attribute']
        del self._control_points[attribute]
        self._control_points[attribute] = []

        for p in range(0, len(obj['points'])):
            point = obj['points'][p]
            self._control_points[attribute].append([point['x'], point['y']])

    def __str__(self):
        """
        Display the serialized representation of the transfer function
        """
        return self.serialize(self)


class Brayns(object):

    def __init__(self, url):
        """
        Setup brayns context
        """
        self._url = url
        self._url_attribute = self._url + '/zerobuf/render/attribute'
        self._url_fov_camera = self._url + '/zerobuf/render/fovcamera'
        self._url_color_map = self._url + '/zerobuf/render/colormap'
        self._url_image_jpeg = self._url + '/lexis/render/imagejpeg'
        self._url_material = self._url + '/zerobuf/render/material'
        self._url_transfer_function = self._url + '/zerobuf/render/transferFunction1D'
        self._url_frame_buffers = self._url + '/zerobuf/render/framebuffers'

    @property
    def camera(self):
        camera = Camera()
        camera.deserialize(self.__request(HTTP_METHOD_GET, self._url_fov_camera))
        return camera

    @camera.setter
    def camera(self, camera):
        """
        Get or set Brayns' camera
        :param camera: Camera to be used by Brayns for the rendering.
        :return If camera parameter is None, the current Brayns camera is returned
        """
        self.__request(HTTP_METHOD_PUT, self._url_fov_camera, camera.serialize())

    @property
    def ambient_occlusion(self):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_AMBIENT_OCCLUSION)

    @ambient_occlusion.setter
    def ambient_occlusion(self, strengh):
        """
        Set ambient occlusion strength. 0 is no occlusion, 1 is full occlusion
        :param strengh: Strength of ambient occlusion [0..1]
        """
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_AMBIENT_OCCLUSION, strengh)

    @property
    def image_jpeg_quality(self):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_JPEG_COMPRESSION)

    @image_jpeg_quality.setter
    def image_jpeg_quality(self, quality):
        """
        Set image JPEG quality. 0 is poor, 100 is high
        :param quality: Image JPEG quality [0..100]
        """
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_JPEG_COMPRESSION, quality)

    @property
    def samples_per_pixel(self):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_SAMPLES_PER_PIXEL)

    @samples_per_pixel.setter
    def samples_per_pixel(self, samples_per_pixel):
        """
        Set the number of samples per pixels
        :param samples_per_pixel: Number of samples per pixel
        """
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_SAMPLES_PER_PIXEL, samples_per_pixel)

    @property
    def background_color(self):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_BACKGROUND_COLOR)

    @background_color.setter
    def background_color(self, color):
        """
        Set the background color for the scene
        :param r: Red component [0..1]
        :param g: Green component [0..1]
        :param b: Blue component [0..1]
        """
        value = str(color[0]) + ' ' + str(color[1]) + ' ' + str(color[2])
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_BACKGROUND_COLOR, value)

    @property
    def window_size(self):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_WINDOW_SIZE)

    @window_size.setter
    def window_size(self, size):
        """
        Set image size
        :param width: Width of image in pixels
        :param height: Height of image in pixels
        """
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_WINDOW_SIZE, str(size[0]) + ' ' + str(size[1]))

    @property
    def image_jpeg_size(self, width, height):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_JPEG_SIZE)

    @image_jpeg_size.setter
    def image_jpeg_size(self, size):
        """
        Set JPEG image size
        :param width: Width of JPEG image in pixels
        :param height: Height of JPEG image in pixels
        """
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_JPEG_SIZE, str(size[0]) + ' ' + str(size[1]))

    @property
    def shadows(self):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_JPEG_SIZE)

    @shadows.setter
    def shadows(self, enabled):
        """
        De/activate rendering of shadows
        :param enabled: True activates shadows, False deactivates them
        """
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_SHADOWS, int(enabled))

    @property
    def soft_shadows(self):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_SOFT_SHADOWS)

    @soft_shadows.setter
    def soft_shadows(self, enabled):
        """
        De/activate rendering of soft shadows. This only works if shadows are already activated
        :param enabled: True activates soft shadows, False deactivates them
        """
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_SOFT_SHADOWS, int(enabled))

    @property
    def epsilon(self):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_EPSILON)

    @epsilon.setter
    def epsilon(self, epsilon):
        """
        Ray-tracers have to deal with the finite precision of computer calculations. Since the
        origin of the reflected ray lies on the surface of the object, there will be an
        intersection point at zero distance. Since we do not want that, all intersection
        distances less than the epsilon value are ignored.
        :param epsilon: Value for epsilon
        """
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_EPSILON, str(epsilon))

    @property
    def timestamp(self):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_TIMESTAMP)

    @timestamp.setter
    def timestamp(self, timestamp):
        """
        Set timestamp attached to the scene. This parameter is typically used for electrical
        simulation or neuron growth scenarios
        :param timestamp: Value for scene timestamp
        """
        self.__attribute(BRAYNS_ATTRIBUTE_TIMESTAMP, str(timestamp))

    @property
    def shader(self):
        return self._url_attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_MATERIAL)

    @shader.setter
    def shader(self, shader):
        """
        Set the shader to be used by the renderer
        :param shader: Shader to be used. Possible values are SHADER_DIFFUSE for Phong & Blinn
        shading, SHADER_ELECTRON for electron shading (Fast and independent from light sources),
         and SHADER_NOSHADING for no shading at all (usually used together with ambient occlusion
         to produce bright images)
        """
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_MATERIAL, shader)

    @property
    def renderer(self):
        return self.__attribute(HTTP_METHOD_GET, BRAYNS_ATTRIBUTE_RENDERER)

    @renderer.setter
    def renderer(self, renderer):
        """
        Set renderer to be used by Brayns
        :param renderer: Renderer to be used. Possible values are DEFAULT_RENDERER for common case,
        RENDERER_PROXIMITY_DETECTION for interactive touch detection feature, RENDERER_SIMULATION
        for electrical simulation
        """
        self.__attribute(HTTP_METHOD_PUT, BRAYNS_ATTRIBUTE_RENDERER, renderer)

    @property
    def material(self):
        return self.__request(HTTP_METHOD_GET, self._url_material)

    @material.setter
    def material(self, material):
        """
        Set material to Brayns
        :param material: Material definition
        """
        self.__request(HTTP_METHOD_PUT, self._url_material, material.serialize())

    @property
    def image_jpeg(self):
        """
        Get JPEG image from Brayns
        :return: Pillow Image object, None if image could not be retrieved
        """
        response = self.__request(HTTP_METHOD_GET, self._url_image_jpeg)
        if response is None:
            return None
        payload = json.loads(response)
        jpeg_image = Image.open(BytesIO(base64.b64decode(payload['data'])))
        return jpeg_image

    @property
    def color_frame_buffer(self):
        """
        Get color frame buffer for Brayns
        :return: Pillow Image object, None if image could not be retrieved or frame_type is invalid
        """
        response = self.__request(HTTP_METHOD_GET, self._url_frame_buffers)
        if response is None:
            return None
        payload = json.loads(response)
        size = [payload['width'], payload['height']]
        return Image.frombytes('RGBA', size, base64.b64decode(payload['diffuse']))

    @property
    def depth_frame_buffer(self):
        """
        Get depth frame buffer for Brayns
        :return: Pillow Image object, None if image could not be retrieved or frame_type is invalid
        """
        response = self.__request(HTTP_METHOD_GET, self._url_frame_buffers)
        if response is None:
            return None
        payload = json.loads(response)
        size = [payload['width'], payload['height']]
        return Image.frombytes('I;16', size, base64.b64decode(payload['depth']))

    @property
    def transfer_function(self):
        return self.__request(HTTP_METHOD_GET, self._url_transfer_function)

    @transfer_function.setter
    def transfer_function(self, transfer_function):
        """
        Set transfer function to Brayns for simulation renderer
        :param transfer_function: Transfer function definition
        """
        for attribute in transfer_function.get_attributes():
            self.__request(HTTP_METHOD_PUT, self._url_transfer_function, transfer_function.serialize(attribute))

    def __attribute(self, method, key, value = None):
        """
        Generic method to set an attribute to Brayns
        :param key: Name of the attribute
        :param value: Value of the attribute
        """
        if method == HTTP_METHOD_PUT:
            payload = {'key': key, 'value': value}
            return self.__request(method, self._url_attribute, json.dumps(payload))
        else:
            payload = {'key': key, 'value': ''}
            response = self.__request(method, self._url_attribute, json.dumps(payload))
            if response is not None:
                obj = json.loads()
                response = str(obj['value'])
            return response

    @staticmethod
    def __request(method, url, body=None):
        """
        Queries the HTTP REST interface of Brayns for a given url and method
        :param url: Brayns url
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
                    request = requests.put(url)
                else:
                    request = requests.put(url, data=body)
            elif method == HTTP_METHOD_GET:
                request = requests.get(url)
                response = str(request.text)
            request.close()
        except requests.exceptions.ConnectionError:
            print('ERROR: Failed to connect to Brayns, did you start it with the '\
                  '--zeroeq-http-server command line option?')
        return response
