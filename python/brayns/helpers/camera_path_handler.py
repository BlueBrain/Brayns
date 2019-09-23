#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2019, Blue Brain Project
#                          Cyrille Favreau <cyrille.favreau@epfl.ch>
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

"""Provides a class that ease the definition of smoothed camera paths"""


class CameraPathHandler:
    """Camera path handler"""

    def __init__(self, control_points, nb_steps_per_sequence, smoothing_size):
        """
        Create a new Camera Path Handler instance

        :param list control_points: List of control points
        :param int nb_steps_per_sequence: Number of frames between two control points
        :param int smoothing_size: Number of steps to be considered for the smoothing of the path
        """
        self._control_points = control_points
        self._nb_steps_per_sequence = nb_steps_per_sequence
        self._smoothing_size = int(smoothing_size)
        self._smoothed_key_frames = list()
        self._build_path()

    # pylint: disable=R0914
    def _build_path(self):
        """Builds a smoothed path according to class member contents"""
        origins = list()
        directions = list()
        ups = list()
        aperture_radii = list()
        focus_distances = list()

        for s in range(len(self._control_points)-1):

            p0 = self._control_points[s]
            p1 = self._control_points[s + 1]

            for i in range(self._nb_steps_per_sequence):
                origin = [0, 0, 0]
                direction = [0, 0, 0]
                up = [0, 0, 0]

                t_origin = [0, 0, 0]
                t_direction = [0, 0, 0]
                t_up = [0, 0, 0]
                for k in range(3):
                    t_origin[k] = \
                        (p1['origin'][k] - p0['origin'][k]) / \
                        float(self._nb_steps_per_sequence)
                    t_direction[k] = \
                        (p1['direction'][k] - p0['direction'][k]) / \
                        float(self._nb_steps_per_sequence)
                    t_up[k] = \
                        (p1['up'][k] - p0['up'][k]) / \
                        float(self._nb_steps_per_sequence)

                    origin[k] = p0['origin'][k] + t_origin[k] * float(i)
                    direction[k] = p0['direction'][k] + t_direction[k] * float(i)
                    up[k] = p0['up'][k] + t_up[k] * float(i)

                t_aperture_radius = (p1['apertureRadius'] - p0['apertureRadius']) / float(self._nb_steps_per_sequence)
                aperture_radius = p0['apertureRadius'] + t_aperture_radius * float(i)

                t_focus_distance = (p1['focusDistance'] - p0['focusDistance']) / float(self._nb_steps_per_sequence)
                focus_distance = p0['focusDistance'] + t_focus_distance * float(i)

                origins.append(origin)
                directions.append(direction)
                ups.append(up)
                aperture_radii.append(aperture_radius)
                focus_distances.append(focus_distance)

        nb_frames = len(origins)
        for i in range(nb_frames):
            o = [0, 0, 0]
            d = [0, 0, 0]
            u = [0, 0, 0]
            aperture_radius = 0.0
            focus_distance = 0.0
            for j in range(int(self._smoothing_size)):
                index = int(max(0, min(i + j - self._smoothing_size / 2, nb_frames - 1)))
                for k in range(3):
                    o[k] = o[k] + origins[index][k]
                    d[k] = d[k] + directions[index][k]
                    u[k] = u[k] + ups[index][k]
                aperture_radius = aperture_radius + aperture_radii[index]
                focus_distance = focus_distance + focus_distances[index]
            self._smoothed_key_frames.append([
                (o[0] / self._smoothing_size,
                 o[1] / self._smoothing_size,
                 o[2] / self._smoothing_size),
                (d[0] / self._smoothing_size,
                 d[1] / self._smoothing_size,
                 d[2] / self._smoothing_size),
                (u[0] / self._smoothing_size,
                 u[1] / self._smoothing_size,
                 u[2] / self._smoothing_size),
                aperture_radius / self._smoothing_size,
                focus_distance / self._smoothing_size])
        last = self._control_points[len(self._control_points)-1]
        self._smoothed_key_frames.append(
            (last['origin'], last['direction'], last['up'], last['apertureRadius'], last['focusDistance']))

    def get_nb_frames(self):
        """
        Gets the number of smoothed frames

        :return: The number of smoothed frames
        :rtype: int
        """
        return len(self._smoothed_key_frames)

    def get_key_frame(self, frame):
        """
        Gets the smoothed camera information for the given frame

        :param int frame: Frame number
        :return: The smoothed camera information for the given frame
        :rtype: list
        :raise KeyError: Specified frame is out of range
        """
        if frame < len(self._smoothed_key_frames):
            return self._smoothed_key_frames[frame]
        raise KeyError
