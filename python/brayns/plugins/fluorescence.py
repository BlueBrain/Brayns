#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020, Blue Brain Project
# Nadir Román Guerrero <nadir.romanguerrero@epfl.ch>
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
# All rights reserved. Do not distribute without further notice.

"""Provides a class that wraps the API exposed by the braynsFluorescence plug-in"""


class Fluorescence:
    """Fluorescence, a class that wraps the API exposed by the braynsFluorescence plug-in"""

    # Network defaults
    DEFAULT_RESPONSE_TIMEOUT = 360

    def __init__(self, client):
        """Creates a new Fluorescence object"""
        self.cli = client.rockets_client

    # pylint: disable=R0913
    def add_rectangle_sensor(self, fov, x_pixels, y_pixels, x, y, height,
                             rotation=(0, 1, 0, 0), translation=(0, 0, 0),
                             scale=(1, 1, 1), reference=""):
        """
        Adds a rectangle sensor to the scene

        :param float fov: Field of view of the sensor
        :param int x_pixels: pixel width to capture
        :param int y_pixels: pixel height to capture
        :param float x: Rectangle lenght in the X axis
        :param float y: Rectangle lenght in the Y axis
        :param float height: Rectangle depth in the Z axys
        :param list rotation: Sensor shape rotation (angle + axis)
        :param list translation: Sensor shape translation
        :param list scale: Sensor shape scale
        :param str reference: Sensor reference name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["rotation"] = rotation
        params["translation"] = translation
        params["scale"] = scale
        params["reference"] = reference
        params["xpixels"] = x_pixels
        params["ypixels"] = y_pixels
        params["fov"] = fov
        params["height"] = height
        params["x"] = x
        params["y"] = y

        return self.cli.request("add-rectangle-sensor", params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913
    def add_disk_sensor(self, fov, x_pixels, y_pixels, height, radius,
                        inner_radius, phi_max, rotation=(0, 1, 0, 0),
                        translation=(0, 0, 0), scale=(1, 1, 1), reference=""):
        """
        Adds a rectangle sensor to the scene

        :param float fov: Field of view of the sensor
        :param int x_pixels: pixel width to capture
        :param int y_pixels: pixel height to capture
        :param float height: Disk height
        :param float radius: Disk main radius
        :param float inner_radius: Disk inner radius (hole in center)
        :param float phi_max: Coverage of the circle (0 to 360)
        :param list rotation: Sensor shape rotation (angle + axis)
        :param list translation: Sensor shape translation
        :param list scale: Sensor shape scale
        :param str reference: Sensor reference name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["rotation"] = rotation
        params["translation"] = translation
        params["scale"] = scale
        params["reference"] = reference
        params["xpixels"] = x_pixels
        params["ypixels"] = y_pixels
        params["fov"] = fov
        params["height"] = height
        params["radius"] = radius
        params["innerRadius"] = inner_radius
        params["phi"] = phi_max

        return self.cli.request("add-disk-sensor", params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913
    def add_fluorescent_volume(self, fex, fem, epsilon, c, yieldv,
                               gf, p0=(0, 0, 0), p1=(0, 0, 0), name=""):
        """
        Adds a Fluorescent volume to the scene

        :param list fex: excitation spectrum
        :param list fem: emission spectrum
        :param float epsilon: fluorescence epsilon
        :param float c: fluorescence concentration
        :param float yieldv: fluorescence quantun yield
        :param float gf: fluorescence anisotropy
        :param list p0: minimun bound of the enclosing volume
        :param list p1: maximun bound of the enclosing volume
        :param str name: scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["p0"] = p0
        params["p1"] = p1
        params["fex"] = fex
        params["fem"] = fem
        params["epsilon"] = epsilon
        params["c"] = c
        params["yield"] = yieldv
        params["gf"] = gf
        params["name"] = name

        return self.cli.request("add-fluorescent-volume", params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913,R0911
    def add_fluorescent_annotated_volume(self, g, ntags, fexs, fems, epsilons,
                                         cs, yields, gfs, prefix, p0=(0, 0, 0),
                                         p1=(0, 0, 0), name=""):
        """
        Adds a Fluorescent annotated volume to the scene

        :param float g: anisotropy factor
        :param int ntags: number of tags held by this volume
        :param list fexs: list of excitation spectrums
        :param list fems: list of emission spectrums
        :param list epsilons: list of fluorescence epsilons
        :param list cs: list of fluorescence concentrations
        :param list yields: list of fluorescence quantun yields
        :param list gfs: list of fluorescence anisotropy factors
        :param str prefix: path to the volume density file
        :param list p0: minimun bound of the enclosing volume
        :param list p1: maximun bound of the enclosing volume
        :param str name: scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["p0"] = p0
        params["p1"] = p1
        params["g"] = g
        params["ntags"] = ntags
        params["fexs"] = fexs
        params["fems"] = fems
        params["epsilons"] = epsilons
        params["cs"] = cs
        params["yields"] = yields
        params["gfs"] = gfs
        params["prefix"] = prefix
        params["name"] = name

        return self.cli.request("add-fluorescent-ann-volume", params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913
    def add_fluorescent_binary_volume(self, g, fex, fem, epsilon, c, yieldv,
                                      gf, prefix, p0=(0, 0, 0), p1=(0, 0, 0),
                                      name=""):
        """
        Adds a Fluorescent binary volume to the scene

        :param float g: anisotropy coefficent
        :param list fex: excitation spectrum
        :param list fem: emission spectrum
        :param float epsilon: fluorescence epsilon
        :param float c: fluorescence concentration
        :param float yieldv: fluorescence quantun yield
        :param float gf: fluorescence anisotropy
        :param str prefix: path to the volume density file
        :param list p0: minimun bound of the enclosing volume
        :param list p1: maximun bound of the enclosing volume
        :param str name: scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["p0"] = p0
        params["p1"] = p1
        params["g"] = g
        params["fex"] = fex
        params["fem"] = fem
        params["epsilon"] = epsilon
        params["c"] = c
        params["yield"] = yieldv
        params["gf"] = gf
        params["prefix"] = prefix
        params["name"] = name

        return self.cli.request("add-fluorescent-bin-volume", params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913
    def add_fluorescent_grid_volume_fromfile(self, g, fex, fem, epsilon, c,
                                             yieldv, gf, prefix, p0=(0, 0, 0),
                                             p1=(0, 0, 0), name=""):
        """
        Adds a Fluorescent grid volume to the scene loaded from a file

        :param float g: anisotropy coefficent
        :param list fex: excitation spectrum
        :param list fem: emission spectrum
        :param float epsilon: fluorescence epsilon
        :param float c: fluorescence concentration
        :param float yieldv: fluorescence quantun yield
        :param float gf: fluorescence anisotropy
        :param str prefix: path to the volume density file
        :param list p0: minimun bound of the enclosing volume
        :param list p1: maximun bound of the enclosing volume
        :param str name: scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["p0"] = p0
        params["p1"] = p1
        params["g"] = g
        params["fex"] = fex
        params["fem"] = fem
        params["epsilon"] = epsilon
        params["c"] = c
        params["yield"] = yieldv
        params["gf"] = gf
        params["prefix"] = prefix
        params["name"] = name

        return self.cli.request("add-fluorescent-grid-file-volume",
                                params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913,R0914
    def add_fluorescent_grid_volume(self, g, fex, fem, epsilon, c, yieldv, gf,
                                    density, nx, ny, nz, p0=(0, 0, 0),
                                    p1=(0, 0, 0), name=""):
        """
        Adds a Fluorescent grid volume to the scene using the input data

        :param float g: anisotropy coefficent
        :param list fex: excitation spectrum
        :param list fem: emission spectrum
        :param float epsilon: fluorescence epsilon
        :param float c: fluorescence concentration
        :param float yieldv: fluorescence quantun yield
        :param float gf: fluorescence anisotropy
        :param list density: list of densities on a per cell basis
        :param int nx: volume grid width
        :param int ny: volume grid height
        :param int nz: volume grid depth
        :param list p0: minimun bound of the enclosing volume
        :param list p1: maximun bound of the enclosing volume
        :param str name: scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["p0"] = p0
        params["p1"] = p1
        params["g"] = g
        params["fex"] = fex
        params["fem"] = fem
        params["epsilon"] = epsilon
        params["c"] = c
        params["yield"] = yieldv
        params["gf"] = gf
        params["density"] = density
        params["nx"] = nx
        params["ny"] = ny
        params["nz"] = nz
        params["name"] = name

        return self.cli.request("add-fluorescent-grid-volume",
                                params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913,R0914
    def add_fluorescent_scattering_volume(self, g, sig_a, sig_s, fex, fem,
                                          m_weight, epsilon, c, yieldv, gf,
                                          s_scale, f_scale, density, Le,
                                          p0=(0, 0, 0), p1=(0, 0, 0), name=""):
        """
        Adds a Fluorescent scattering volume to the scene

        :param float g: anisotropy coefficent
        :param list sig_a: absorption spectrum
        :param list sig_s: scattering spectrum
        :param list fex: excitation spectrum
        :param list fem: emission spectrum
        :param float m_weight: weight
        :param float epsilon: fluorescence epsilon
        :param float c: fluorescence concentration
        :param float yieldv: fluorescence quantun yield
        :param float gf: fluorescence anisotropy
        :param float s_scale: scattering scaling coefficent
        :param float f_scale: flourescence scaling coefficent
        :param float density: homogeneus density
        :param list Le: Radiance spectrum
        :param list p0: minimun bound of the enclosing volume
        :param list p1: maximun bound of the enclosing volume
        :param str name: scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["p0"] = p0
        params["p1"] = p1
        params["g"] = g
        params["absorption"] = sig_a
        params["scattering"] = sig_s
        params["fex"] = fex
        params["fem"] = fem
        params["mweight"] = m_weight
        params["epsilon"] = epsilon
        params["c"] = c
        params["yield"] = yieldv
        params["sscale"] = s_scale
        params["fscale"] = f_scale
        params["gf"] = gf
        params["density"] = density
        params["Le"] = Le
        params["name"] = name

        return self.cli.request("add-fluorescent-scatter-volume",
                                params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913,R0914
    def add_fluorescent_scattering_grid_volume(self, g, sig_a, sig_s, fex, fem,
                                               m_weight, epsilon, c, yieldv,
                                               gf, s_scale, f_scale, density,
                                               Le, nx, ny, nz, p0=(0, 0, 0),
                                               p1=(0, 0, 0), name=""):
        """
        Adds a Fluorescent scattering grid volume to the scene from input data

        :param float g: anisotropy coefficent
        :param list sig_a: absorption spectrum
        :param list sig_s: scattering spectrum
        :param list fex: excitation spectrum
        :param list fem: emission spectrum
        :param float m_weight: weight
        :param float epsilon: fluorescence epsilon
        :param float c: fluorescence concentration
        :param float yieldv: fluorescence quantun yield
        :param float gf: fluorescence anisotropy
        :param float s_scale: scattering scaling coefficent
        :param float f_scale: flourescence scaling coefficent
        :param float density: homogeneus density
        :param list Le: Radiance spectrum
        :param int nx: volume grid width
        :param int ny: volume grid height
        :param int nz: volume grid depth
        :param list p0: minimun bound of the enclosing volume
        :param list p1: maximun bound of the enclosing volume
        :param str name: scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["p0"] = p0
        params["p1"] = p1
        params["g"] = g
        params["absorption"] = sig_a
        params["scattering"] = sig_s
        params["fex"] = fex
        params["fem"] = fem
        params["mweight"] = m_weight
        params["epsilon"] = epsilon
        params["c"] = c
        params["yield"] = yieldv
        params["sscale"] = s_scale
        params["fscale"] = f_scale
        params["gf"] = gf
        params["density"] = density
        params["Le"] = Le
        params["nx"] = nx
        params["ny"] = ny
        params["nz"] = nz
        params["name"] = name

        return self.cli.request("add-fluorescent-scattergrid-volume",
                                params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)

    # pylint: disable=R0913,R0914
    def add_fluorescent_scattering_volume_fromfile(self, g, sig_a, sig_s, fex,
                                                   fem, m_weight, epsilon, c,
                                                   yieldv, gf, s_scale,
                                                   f_scale, Le,
                                                   prefix, p0=(0, 0, 0),
                                                   p1=(0, 0, 0), name=""):
        """
        Adds a Fluorescent scattering volume to the scene

        :param float g: anisotropy coefficent
        :param list sig_a: absorption spectrum
        :param list sig_s: scattering spectrum
        :param list fex: excitation spectrum
        :param list fem: emission spectrum
        :param float m_weight: weight
        :param float epsilon: fluorescence epsilon
        :param float c: fluorescence concentration
        :param float yieldv: fluorescence quantun yield
        :param float gf: fluorescence anisotropy
        :param float s_scale: scattering scaling coefficent
        :param float f_scale: flourescence scaling coefficent
        :param list Le: Radiance spectrum
        :param str prefix: path to volume file on disk
        :param list p0: minimun bound of the enclosing volume
        :param list p1: maximun bound of the enclosing volume
        :param str name: scene object name
        :return: Result of the request submission
        :rtype: str
        """
        params = dict()
        params["p0"] = p0
        params["p1"] = p1
        params["g"] = g
        params["absorption"] = sig_a
        params["scattering"] = sig_s
        params["fex"] = fex
        params["fem"] = fem
        params["mweight"] = m_weight
        params["epsilon"] = epsilon
        params["c"] = c
        params["yield"] = yieldv
        params["sscale"] = s_scale
        params["fscale"] = f_scale
        params["gf"] = gf
        params["Le"] = Le
        params["prefix"] = prefix
        params["name"] = name

        return self.cli.request("add-fluorescent-scattergridfile-volume",
                                params=params,
                                response_timeout=self.DEFAULT_RESPONSE_TIMEOUT)
