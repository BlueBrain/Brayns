# Copyright (c) 2015-2022 EPFL/Blue Brain Project
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

import brayns


def prepare_quick_render_image(instance: brayns.Instance, frame: int = 0) -> None:
    brayns.update_application(
        instance,
        resolution=brayns.Resolution.full_hd,
        jpeg_quality=100,
    )
    camera = _prepare_camera_and_light(instance)
    brayns.set_camera(instance, camera)
    renderer = brayns.InteractiveRenderer()
    brayns.set_renderer(instance, renderer)
    brayns.set_simulation_frame(instance, frame)


def quick_snapshot(instance: brayns.Instance, path: str, frame: int = 0) -> None:
    snapshot = prepare_quick_snapshot(instance, frame)
    snapshot.save(instance, path)


def prepare_quick_snapshot(instance: brayns.Instance, frame: int = 0) -> brayns.Snapshot:
    camera = _prepare_camera_and_light(instance)
    return _create_snapshot(camera, frame)


def quick_export(instance: brayns.Instance, folder: str, frames: list[int]) -> None:
    camera = _prepare_camera_and_light(instance)
    exporter = _create_exporter(camera, frames)
    exporter.export_frames(instance, folder)


def _prepare_camera_and_light(instance: brayns.Instance) -> brayns.Camera:
    camera = _create_camera(instance)
    _add_light(instance, camera.direction)
    return camera


def _create_camera(instance: brayns.Instance) -> brayns.Camera:
    target = brayns.get_bounds(instance)
    return brayns.look_at(target)


def _add_light(instance: brayns.Instance, direction: brayns.Vector3) -> None:
    brayns.clear_lights(instance)
    light = brayns.DirectionalLight(
        intensity=5,
        direction=direction,
    )
    brayns.add_light(instance, light)


def _create_snapshot(camera: brayns.Camera, frame: int) -> brayns.Snapshot:
    return brayns.Snapshot(
        resolution=brayns.Resolution.full_hd,
        frame=frame,
        camera=camera,
        renderer=brayns.InteractiveRenderer(),
    )


def _create_exporter(camera: brayns.Camera, frames: list[int]) -> brayns.FrameExporter:
    return brayns.FrameExporter(
        frames=brayns.KeyFrame.from_indices(frames, camera.view),
        resolution=brayns.Resolution.full_hd,
        projection=camera.projection,
        renderer=brayns.InteractiveRenderer(),
    )
