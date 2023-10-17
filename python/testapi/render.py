# Copyright (c) 2015-2023 EPFL/Blue Brain Project
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

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

import brayns

from .simple_test_case import SimpleTestCase

DEFAULT_THRESHOLD = 1.0


class ValidationFailed(RuntimeError):
    pass


@dataclass
class RenderSettings:
    resolution: brayns.Resolution | None = brayns.Resolution.full_hd
    camera: brayns.Camera | None = None
    renderer: brayns.Renderer | None = None
    frame: int | None = None
    format: brayns.ImageFormat = brayns.ImageFormat.PNG
    center_camera: bool = True
    add_lights: bool = True
    use_snapshot: bool = True
    accumulate: bool = False
    threshold: float = DEFAULT_THRESHOLD

    @staticmethod
    def raw() -> RenderSettings:
        return RenderSettings(
            resolution=None,
            center_camera=False,
            add_lights=False,
            use_snapshot=False,
        )


def render_and_save(
    context: SimpleTestCase, filename: str, settings: RenderSettings = RenderSettings()
) -> None:
    data = render(context.instance, settings)
    path = _get_full_path(context, filename, settings)
    with path.open("wb") as file:
        file.write(data)


def render_and_validate(
    context: SimpleTestCase, filename: str, settings: RenderSettings = RenderSettings()
) -> None:
    test = render(context.instance, settings)
    path = _get_full_path(context, filename, settings)
    validate_from_file(test, path, settings.threshold)


def render(
    instance: brayns.Instance, settings: RenderSettings = RenderSettings()
) -> bytes:
    if settings.add_lights:
        prepare_lights(instance)
    if settings.use_snapshot:
        return _snapshot(instance, settings)
    return _image(instance, settings)


def prepare_snapshot(
    instance: brayns.Instance, settings: RenderSettings = RenderSettings()
) -> brayns.Snapshot:
    return brayns.Snapshot(
        resolution=settings.resolution,
        camera=get_camera(instance, settings),
        renderer=settings.renderer,
        frame=settings.frame,
    )


def prepare_image(
    instance: brayns.Instance, settings: RenderSettings = RenderSettings()
) -> None:
    if settings.resolution is not None:
        brayns.set_resolution(instance, settings.resolution)
    camera = get_camera(instance, settings)
    if camera is not None:
        brayns.set_camera(instance, camera)
    if settings.renderer is not None:
        brayns.set_renderer(instance, settings.renderer)
    if settings.frame is not None:
        brayns.set_simulation_frame(instance, settings.frame)


def get_camera(
    instance: brayns.Instance, settings: RenderSettings
) -> brayns.Camera | None:
    if not settings.center_camera:
        return settings.camera
    aspect_ratio = 1
    if settings.resolution is not None:
        aspect_ratio = settings.resolution.aspect_ratio
    projection: type[brayns.Projection] = brayns.PerspectiveProjection
    if settings.camera is not None:
        projection = type(settings.camera.projection)
    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=aspect_ratio,
        projection=projection,
    )
    return controller.camera


def prepare_lights(instance: brayns.Instance) -> None:
    brayns.clear_lights(instance)
    ambient = brayns.AmbientLight(0.5)
    brayns.add_light(instance, ambient)
    direction = brayns.Vector3(1, -1, -1)
    directional = brayns.DirectionalLight(10, direction=direction)
    brayns.add_light(instance, directional)


def validate_file(test: Path, ref: Path, threshold: float = DEFAULT_THRESHOLD) -> None:
    with test.open("rb") as file:
        test_data = file.read()
    validate_from_file(test_data, ref, threshold)


def validate_from_file(
    test: bytes, ref: Path, threshold: float = DEFAULT_THRESHOLD
) -> None:
    with ref.open("rb") as file:
        ref_data = file.read()
    _validate_data(test, ref_data, threshold)


def _validate_data(
    test: bytes, ref: bytes, threshold: float = DEFAULT_THRESHOLD
) -> None:
    l1, l2 = len(test), len(ref)
    common = min(l1, l2)
    extra = abs(l1 - l2)
    se = sum((test[i] - test[i]) ** 2 for i in range(common))
    se += extra * 255 * 255
    mse = se / (l1 * l2)
    if mse > threshold:
        raise ValidationFailed(f"Image difference {mse} > {threshold}")


def _get_full_path(
    context: SimpleTestCase, filename: str, settings: RenderSettings
) -> Path:
    extension = settings.format.value
    return context.asset_folder / f"{filename}.{extension}"


def _snapshot(instance: brayns.Instance, settings: RenderSettings) -> bytes:
    snapshot = prepare_snapshot(instance, settings)
    return snapshot.download(instance, settings.format)


def _image(instance: brayns.Instance, settings: RenderSettings) -> bytes:
    prepare_image(instance, settings)
    image = brayns.Image(settings.accumulate)
    info = image.download(instance, settings.format)
    return info.data
