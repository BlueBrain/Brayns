import math
import time
from collections.abc import Callable
from pathlib import Path

import brayns

URI = "sbo-brayns.shapes-registry.org:8200"
SSL = brayns.SslClientContext()

LOADER = brayns.MeshLoader()
PATH = "/home/acfleury/source/test/brain.obj"

IMAGE_COUNT = 2
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


def upload_model(instance: brayns.Instance) -> None:
    path = Path(PATH)
    format = path.suffix[1:]
    with path.open("rb") as file:
        data = file.read()
    LOADER.upload_models(instance, format, data)


def prepare_scene(instance: brayns.Instance) -> None:
    brayns.clear_models(instance)
    upload_model(instance)
    brayns.set_renderer(instance, RENDERER)
    brayns.clear_lights(instance)
    for light in LIGHTS:
        brayns.add_light(instance, light)


def prepare_render(instance: brayns.Instance, resolution: brayns.Resolution) -> None:
    brayns.set_resolution(instance, resolution)
    target = brayns.get_bounds(instance)
    controller = brayns.CameraController(target, resolution.aspect_ratio)
    brayns.set_camera(instance, controller.camera)


def reset_accumulation(instance: brayns.Instance) -> None:
    angle = 360 / IMAGE_COUNT
    rotation = brayns.euler(0, angle, 0, degrees=True)
    view = brayns.get_camera_view(instance)
    view = view.rotate_around_target(rotation)
    brayns.set_camera_view(instance, view)


def render(instance: brayns.Instance) -> None:
    image = brayns.Image(accumulate=False)
    image.render(instance)


def download(instance: brayns.Instance) -> None:
    image = brayns.Image(accumulate=False)
    image.download(instance, IMAGE_FORMAT, render=False)


def render_and_download(instance: brayns.Instance) -> None:
    image = brayns.Image(accumulate=False)
    image.download(instance, IMAGE_FORMAT)


def send_request(instance: brayns.Instance) -> None:
    image = brayns.Image(accumulate=False, force_download=False)
    image.download(instance, IMAGE_FORMAT, render=False)


def profile(
    instance: brayns.Instance,
    function: Callable[[brayns.Instance], None],
) -> list[float]:
    durations = list[float]()
    for _ in range(IMAGE_COUNT):
        reset_accumulation(instance)
        start = time.time()
        function(instance)
        duration = time.time() - start
        durations.append(duration)
    return durations


def get_mean_and_std(data: list[float]) -> tuple[float, float]:
    n = len(data)
    mean = sum(data) / n
    var = sum((i - mean) ** 2 for i in data) / (n - 1)
    return mean, math.sqrt(var)


def run(
    instance: brayns.Instance,
    resolution: brayns.Resolution,
    function: Callable[[brayns.Instance], None],
) -> tuple[float, float]:
    prepare_render(instance, resolution)
    data = profile(instance, function)
    return get_mean_and_std(data)


def print_result(title: str, mean_and_std: tuple[float, float]) -> None:
    mean = 1000 * mean_and_std[0]
    std = 1000 * mean_and_std[1]
    fps = 1 / mean_and_std[0]
    print(f"{title} duration: {mean:.2f}ms (std = {std:.2f}ms) ({fps:.2f}fps)")


def measure(instance: brayns.Instance, resolution: brayns.Resolution) -> None:
    a = run(instance, resolution, lambda instance: render(instance))
    b = run(instance, resolution, lambda instance: download(instance))
    c = run(instance, resolution, lambda instance: render_and_download(instance))
    d = run(instance, resolution, lambda instance: send_request(instance))
    print(f"Resolution: {resolution.width}x{resolution.height}")
    print_result("Render", a)
    print_result("Download", b)
    print_result("Render and download", c)
    print_result("Requests only", d)
    print()


def measure_all() -> None:
    connector = brayns.Connector(URI, SSL)
    with connector.connect() as instance:
        prepare_scene(instance)
        print(f"Image count: {IMAGE_COUNT}")
        print()
        for resolution in RESOLUTIONS:
            measure(instance, resolution)


if __name__ == "__main__":
    measure_all()
