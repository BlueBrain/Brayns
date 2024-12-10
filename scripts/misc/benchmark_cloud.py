import time

import brayns

NODE = "r1i6n15"
URI = f"{NODE}.bbp.epfl.ch:5000"
PATH = "/gpfs/bbp.cscs.ch/project/proj83/jira-tickets/NSETM-1948-extract-hex-O1/data/O1_data/circuit_config.json"


def render(
    instance: brayns.Instance,
    count: int,
    download: bool = True,
    render: bool = True,
) -> None:
    if not render and not download:
        raise ValueError("You fucked up man")

    view = brayns.get_camera_view(instance)

    image = brayns.Image(accumulate=False, force_download=download)

    durations = list[float]()

    for i in range(count):
        increment = 1 if i % 2 == 0 else -1
        view.position += increment * brayns.Axis.z
        brayns.set_camera_view(instance, view)

        start = time.time()

        if download:
            image.download(instance, brayns.ImageFormat.JPEG, render)
        else:
            image.render(instance)

        duration = time.time() - start

        durations.append(duration)

    average = sum(durations) / len(durations)
    print(f"Average frame duration = {average:.2f}s")
    print(f"Average fps = {1 / average:.2f}fps")


connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    population = brayns.SonataNodePopulation(
        name="S1nonbarrel_neurons",
        nodes=brayns.SonataNodes.from_density(1),
        morphology=brayns.Morphology(load_dendrites=True),
    )

    loader = brayns.SonataLoader([population])

    loader.load_models(instance, PATH)

    lights = [
        brayns.DirectionalLight(direction=brayns.Vector3.one),
        brayns.DirectionalLight(direction=-brayns.Vector3.one),
        brayns.DirectionalLight(direction=brayns.Vector3.one),
        brayns.DirectionalLight(direction=-brayns.Vector3.one),
    ]
    for light in lights:
        brayns.add_light(instance, light)

    resolution = brayns.Resolution.full_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer(samples_per_pixel=1)

    brayns.set_resolution(instance, resolution)
    brayns.set_camera(instance, camera)
    brayns.set_renderer(instance, renderer)

    count = 50
    print("Everything")
    render(instance, count)
    print()
    print("Render only")
    render(instance, count, download=False)
    print()
    print("Download only")
    render(instance, count, render=False)
