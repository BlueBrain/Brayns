import time

import brayns

URI = "r1i7n32.bbp.epfl.ch:5000"

LOADER = brayns.MeshLoader()
PATH = "/gpfs/bbp.cscs.ch/project/proj3/tolokoban/brain.obj"

IMAGE_COUNT = 10
IMAGE_FORMAT = brayns.ImageFormat.JPEG
ACCUMULATE = False
RESOLUTIONS = [brayns.Resolution.full_hd, brayns.Resolution.full_hd // 4]

RENDERER = brayns.InteractiveRenderer(
    samples_per_pixel=8,
    max_ray_bounces=3,
    enable_shadows=False,
    ambient_occlusion_samples=0,
)

LIGHTS = [
    brayns.AmbientLight(intensity=0.4),
    brayns.DirectionalLight(intensity=1, direction=brayns.Vector3.one),
    brayns.DirectionalLight(intensity=2, direction=-brayns.Vector3.one),
]


def prepare_scene(instance: brayns.Instance) -> None:
    brayns.clear_models(instance)
    LOADER.load_models(instance, PATH)
    brayns.set_renderer(instance, RENDERER)
    brayns.clear_lights(instance)
    for light in LIGHTS:
        brayns.add_light(instance, light)


def prepare_render(instance: brayns.Instance, resolution: brayns.Resolution) -> None:
    brayns.set_resolution(instance, resolution)
    target = brayns.get_bounds(instance)
    controller = brayns.CameraController(target, resolution.aspect_ratio)
    brayns.set_camera(instance, controller.camera)


def disable_render(instance: brayns.Instance) -> None:
    image = brayns.Image(accumulate=True, force_download=False)
    image.render(instance)


def trigger_render(instance: brayns.Instance) -> None:
    angle = 360 / IMAGE_COUNT
    rotation = brayns.euler(0, angle, 0, degrees=True)
    view = brayns.get_camera_view(instance)
    view = view.rotate_around_target(rotation)
    brayns.set_camera_view(instance, view)


def render_frame(instance: brayns.Instance, download: bool) -> None:
    image = brayns.Image(accumulate=ACCUMULATE, force_download=download)
    if download:
        image.download(instance, IMAGE_FORMAT)
        return
    image.render(instance)


def render_and_measure(
    instance: brayns.Instance,
    resolution: brayns.Resolution,
    render: bool,
    download: bool,
) -> float:
    prepare_render(instance, resolution)
    durations = list[float]()
    for _ in range(IMAGE_COUNT):
        trigger_render(instance) if render else disable_render(instance)
        start = time.time()
        render_frame(instance, download)
        duration = time.time() - start
        durations.append(duration)
    return sum(durations) / len(durations)


def print_duration(title: str, duration: float) -> None:
    print(f"{title} duration: {1000 * duration:.2f}ms ({1 / duration:.2f}fps)")


def run(instance: brayns.Instance, resolution: brayns.Resolution) -> None:
    render = render_and_measure(instance, resolution, render=True, download=False)
    download = render_and_measure(instance, resolution, render=False, download=True)
    both = render_and_measure(instance, resolution, render=True, download=True)
    print(f"Resolution: {resolution.width}x{resolution.height}")
    print_duration("Render", render)
    print_duration("Download", download)
    print_duration("Render and download", both)
    print()


def main() -> None:
    connector = brayns.Connector(URI)
    with connector.connect() as instance:
        prepare_scene(instance)
        for resolution in RESOLUTIONS:
            run(instance, resolution)


if __name__ == "__main__":
    main()
