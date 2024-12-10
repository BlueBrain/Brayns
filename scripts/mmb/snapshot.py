import logging
import sys
from dataclasses import dataclass

import brayns

from parsing import cfg_to_argv, parse_argv, parse_cfg


@dataclass
class Settings:

    executable: str = 'braynsService'
    library_path: str = ''
    circuit: str = ''
    radius_multiplier: float = 10
    load_soma: bool = True
    load_dendrites: bool = True
    resolution: brayns.Resolution = brayns.Resolution.full_hd
    camera_type: str = brayns.PerspectiveProjection.name
    camera_translation: brayns.Vector3 = brayns.Vector3.zero
    camera_rotation: brayns.Rotation = brayns.CameraRotation.front
    ambient_light_intensity: float = 0.5
    directional_light_intensity: float = 9
    light_rotation: brayns.Rotation = brayns.Rotation.identity
    renderer_type: str = brayns.InteractiveRenderer.name
    sample_per_pixel: int = 1
    background_color: brayns.Color4 = brayns.Color4.bbp_background.transparent
    simulation_frame: int = 0
    save_as: str = ''


PROJECTIONS: list[type[brayns.Projection]] = [
    brayns.PerspectiveProjection,
    brayns.OrthographicProjection,
]

RENDERERS: list[type[brayns.Renderer]] = [
    brayns.InteractiveRenderer,
    brayns.ProductionRenderer,
]


def start(settings: Settings) -> brayns.Manager:
    service = brayns.Service(
        uri='localhost:5000',
        executable=settings.executable,
        env={'LD_LIBRARY_PATH': settings.library_path},
    )
    connector = brayns.Connector(
        uri='localhost:5000',
        logger=brayns.Logger(logging.WARN),
        max_attempts=None,
    )
    return brayns.start(service, connector)


def load_model(instance: brayns.Instance, settings: Settings) -> None:
    loader = brayns.BbpLoader(
        morphology=brayns.Morphology(
            radius_multiplier=settings.radius_multiplier,
            load_soma=settings.load_soma,
            load_dendrites=settings.load_dendrites,
        ),
    )
    loader.load_models(instance, settings.circuit)


def create_projection(settings: Settings) -> brayns.Projection:
    name = settings.camera_type
    for projection in PROJECTIONS:
        if projection.name == name:
            return projection()
    raise ValueError(f'Invalid camera type: "{name}"')


def create_renderer(settings: Settings) -> brayns.Renderer:
    name = settings.renderer_type
    for renderer in RENDERERS:
        if renderer.name == name:
            return renderer(
                samples_per_pixel=settings.sample_per_pixel,
                background_color=settings.background_color,
            )
    raise ValueError(f'Invalid renderer type: "{name}"')


def create_camera(instance: brayns.Instance, settings: Settings) -> brayns.Camera:
    target = brayns.get_bounds(instance)
    controller = brayns.CameraController(
        target=target,
        aspect_ratio=settings.resolution.aspect_ratio,
        translation=settings.camera_translation * target.size,
        rotation=settings.camera_rotation,
        projection=lambda: create_projection(settings),
    )
    return controller.camera


def upload_settings(instance: brayns.Instance, settings: Settings) -> None:
    brayns.set_resolution(instance, settings.resolution)
    brayns.set_camera(instance, create_camera(instance, settings))
    brayns.set_renderer(instance, create_renderer(settings))
    brayns.set_simulation_frame(instance, settings.simulation_frame)


def add_lights(instance: brayns.Instance, settings: Settings) -> None:
    brayns.add_light(instance, brayns.AmbientLight(
        intensity=settings.ambient_light_intensity
    ))
    rotation = settings.light_rotation.combine(settings.camera_rotation)
    brayns.add_light(instance, brayns.DirectionalLight(
        intensity=settings.directional_light_intensity,
        direction=rotation.apply(-brayns.Axis.z),
    ))


def take_snapshot(instance: brayns.Instance, settings: Settings) -> None:
    snapshot = brayns.Snapshot()
    snapshot.save(instance, settings.save_as)


def run(settings: Settings) -> None:
    with start(settings) as (process, instance):
        load_model(instance, settings)
        upload_settings(instance, settings)
        add_lights(instance, settings)
        take_snapshot(instance, settings)


def run_from_cfg(filename: str) -> None:
    settings = parse_cfg(filename, Settings)
    run(settings)


def run_from_argv() -> None:
    settings = parse_argv(Settings)
    run(settings)


def main() -> None:
    argv = sys.argv
    if len(argv) < 2:
        raise ValueError('Expected configuration file in sys.argv[1]')
    if len(argv) == 2:
        filename = sys.argv[1]
        return run_from_cfg(filename)
    return run_from_argv()


if __name__ == '__main__':
    main()
