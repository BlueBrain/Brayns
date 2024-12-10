from pathlib import Path
from threading import Thread
import brayns
from PIL import Image

PATH = "/gpfs/bbp.cscs.ch/project/proj112/circuits/CA1/20211110-BioM/sonata/circuit_config.json"
POPULATION = "hippocampus_neurons"

COLORS = {
    "SLM": brayns.Color4(0.8, 0.8, 0.0),
    "SO": brayns.Color4(0.8, 0.5, 0.1),
    "SP": brayns.Color4(0.3, 0.5, 0.7),
    "SR": brayns.Color4(0.8, 0.5, 0.8),
}

LIGHTS = [
    brayns.AmbientLight(intensity=0.25),
    brayns.DirectionalLight(intensity=9, direction=brayns.Vector3(0, 0, -1)),
]

DENSITY = 0.5

MODEL_ROTATION = brayns.euler(180, -90, 0, degrees=True)

ASPECT = 1
DISTANCE_FACTOR = 0.83
TRANSLATION_X = -130
TRANSLATION_Y = 100

DIVISOR = 4

TEST = False

BACKGROUND = brayns.Color4.black.transparent

if TEST:
    PIXELS = 4000
    RENDERER = brayns.InteractiveRenderer(background_color=BACKGROUND)
else:
    PIXELS = 40_000
    RENDERER = brayns.ProductionRenderer(
        samples_per_pixel=128,
        max_ray_bounces=5,
        background_color=BACKGROUND,
    )

OUTPUT = "/home/acfleury/source/test/scripts/hippo"
TILES = "tiles/{index}.png"

with open("/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.txt") as file:
    NODES = file.read().split()

assert len(NODES) == DIVISOR**2


def add_lights(instance: brayns.Instance) -> None:
    brayns.clear_lights(instance)

    for light in LIGHTS:
        brayns.add_light(instance, light)


def load_model(instance: brayns.Instance) -> brayns.Model:
    loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name=POPULATION,
                nodes=brayns.SonataNodes.from_density(DENSITY),
                morphology=brayns.Morphology(
                    radius_multiplier=2,
                    load_soma=True,
                    load_dendrites=True,
                ),
            )
        ]
    )

    return loader.load_models(instance, PATH)[0]


def rotate_model(instance: brayns.Instance, model: brayns.Model) -> brayns.Model:
    transform = brayns.Transform(rotation=MODEL_ROTATION)
    return brayns.update_model(instance, model.id, transform)


def color_model(instance: brayns.Instance, model: brayns.Model) -> None:
    method = brayns.CircuitColorMethod.LAYER
    brayns.color_model(instance, model.id, method, COLORS)


def load_scene(instance: brayns.Instance) -> brayns.Model:
    brayns.clear_models(instance)
    model = load_model(instance)
    model = rotate_model(instance, model)
    color_model(instance, model)
    return model


def get_tile(index: int) -> tuple[brayns.Vector2, brayns.Vector2]:
    step = 1 / DIVISOR
    row, column = divmod(index, DIVISOR)
    i = row * step
    j = column * step
    return (brayns.Vector2(j, 1 - i), brayns.Vector2(j + step, 1 - i - step))


def get_path(index: int) -> Path:
    return Path(OUTPUT) / TILES.format(index=index)


def snapshot(
    instance: brayns.Instance,
    model: brayns.Model,
    tile: tuple[brayns.Vector2, brayns.Vector2],
    output: Path,
) -> None:
    pixels = PIXELS // DIVISOR
    resolution = brayns.Resolution(pixels, pixels)

    controller = brayns.CameraController(model.bounds, ASPECT)
    camera = controller.camera

    camera.distance *= DISTANCE_FACTOR

    translation = camera.right * TRANSLATION_X + camera.real_up * TRANSLATION_Y
    camera = camera.translate(translation)

    camera.image_start = tile[0]
    camera.image_end = tile[1]

    snapshot = brayns.Snapshot(resolution, camera, RENDERER)

    task = snapshot.save_task(instance, str(output))

    for progress in task:
        print(f"{str(output)}: {100 * progress.amount:.2f}")

    task.wait_for_result()


def run(node_index: int) -> None:
    node = NODES[node_index]

    uri = f"{node}:5000"
    connector = brayns.Connector(uri)

    with connector.connect() as instance:
        models = brayns.get_models(instance)

        if not models:
            model = load_scene(instance)
        else:
            model = next(model for model in models if model.type == "neurons")

        add_lights(instance)
        color_model(instance, model)

        tile = get_tile(node_index)
        path = get_path(node_index)

        snapshot(instance, model, tile, path)


def combine(filenames: list[Path]) -> None:
    images = [Image.open(filename) for filename in filenames]

    width = sum(images[i].width for i in range(DIVISOR))
    height = sum(images[i * DIVISOR].height for i in range(DIVISOR))

    output = Image.new("RGBA", (width, height))

    offset_x = 0
    offset_y = 0

    for i in range(DIVISOR):
        for j in range(DIVISOR):
            image = images[i * DIVISOR + j]
            region = (
                offset_x,
                offset_y,
                offset_x + image.width,
                offset_y + image.height,
            )
            output.paste(image, region)
            offset_x += image.width

        offset_x = 0
        offset_y += image.height

    output.save(Path(OUTPUT) / "combined.png")


def main() -> None:
    node_count = len(NODES)

    threads = [Thread(target=run, args=(index,)) for index in range(node_count)]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    paths = [get_path(i) for i in range(node_count)]

    combine(paths)


if __name__ == "__main__":
    main()
