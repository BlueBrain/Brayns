import bluepy
from brayns import *

MICROCIRCUIT = "/gpfs/bbp.cscs.ch/project/proj1/circuits/SomatosensoryCxS1-v5.r0/O1/merged_circuit/CircuitConfig"
THALAMUS = "/gpfs/bbp.cscs.ch/project/proj82/simulations/Thalamus/20210406/WholeTH/no_hold_1000stimRTs/BlueConfig"
HIPPOCAMPUS = "/gpfs/bbp.cscs.ch/project/proj112/circuits/CA1/20211110-BioM/CircuitConfig"
SSCX = "/gpfs/bbp.cscs.ch/project/proj64/circuits/S1.v6a/20171206/CircuitConfig"
NCX = "/gpfs/bbp.cscs.ch/data/scratch/proj82/circuits/M7/CircuitConfig"
FULL_BRAIN = "/gpfs/bbp.cscs.ch/data/scratch/proj134/home/king/BBPP134-15/2022.10.28/simulation_config_70m_70m.json"
FULL_BRAIN_POPULATION = "root_neurons"
HUMAN_COLUMN = "/gpfs/bbp.cscs.ch/project/proj71/circuits/O1/20200422/CircuitConfig"

EXECUTABLE = "braynsService"
HOST = "localhost:5000"
RENDER_RESULT = "/gpfs/bbp.cscs.ch/project/proj3/nadir/renders/allhands"
RESOLUTION = Resolution.production

LAYER_COLORS = {
    "1": Color4(1.0, 0.9490196078431372, 0.0),
    "2": Color4(0.9686274509803922, 0.5803921568627451, 0.11372549019607843),
    "3": Color4(0.8784313725490196, 0.1843137254901961, 0.3803921568627451),
    "4": Color4(0.9882352941176471, 0.6078431372549019, 0.9921568627450981),
    "5": Color4(0.40784313725490196, 0.6588235294117647, 0.8784313725490196),
    "6": Color4(0.4235294117647059, 0.9019607843137255, 0.3843137254901961),
}

LAYER_COLORS_2 = {
    "0": Color4(1.0, 0.9490196078431372, 0.0),
    "1": Color4(0.9686274509803922, 0.5803921568627451, 0.11372549019607843),
    "2": Color4(0.8784313725490196, 0.1843137254901961, 0.3803921568627451),
    "3": Color4(0.9882352941176471, 0.6078431372549019, 0.9921568627450981),
    "4": Color4(0.40784313725490196, 0.6588235294117647, 0.8784313725490196),
    "5": Color4(0.4235294117647059, 0.9019607843137255, 0.3843137254901961),
}

THALAMUS_LAYER_COLORS = {
    "0": Color4(1.0, 0.9490196078431372, 0.0),
    "1": Color4(0.40784313725490196, 0.6588235294117647, 0.8784313725490196),
}

HIPPOCAMPUS_LAYER_COLORS = {
    "0": Color4(1.0, 0.9490196078431372, 0.0),
    "1": Color4(0.9686274509803922, 0.5803921568627451, 0.11372549019607843),
    "2": Color4(0.40784313725490196, 0.6588235294117647, 0.8784313725490196),
    "3": Color4(0.9882352941176471, 0.6078431372549019, 0.9921568627450981),
}

FREE_COLORS = (
    Color4(1.0, 0.9490196078431372, 0.0, 1.0),
    Color4(0.9686274509803922, 0.5803921568627451, 0.11372549019607843, 1.0),
    Color4(0.8784313725490196, 0.1843137254901961, 0.3803921568627451, 1.0),
    Color4(0.9882352941176471, 0.6078431372549019, 0.9921568627450981, 1.0),
    Color4(0.40784313725490196, 0.6588235294117647, 0.8784313725490196, 1.0),
    Color4(0.4235294117647059, 0.9019607843137255, 0.3843137254901961, 1.0),
    Color4(0.6509803921568628, 0.33725490196078434, 0.1568627450980392, 1.0),
    Color4(0.9686274509803922, 0.5058823529411764, 0.7490196078431373, 1.0),
    Color4(0.6, 0.6, 0.6, 1.0),
    Color4(0.8941176470588236, 0.10196078431372549, 0.10980392156862745, 1.0),
    Color4(0.21568627450980393, 0.49411764705882355, 0.7215686274509804, 1.0),
    Color4(0.30196078431372547, 0.6862745098039216, 0.2901960784313726, 1.0),
    Color4(0.596078431372549, 0.3058823529411765, 0.6392156862745098, 1.0),
    Color4(1.0, 0.4980392156862745, 0.0, 1.0),
    Color4(1.0, 1.0, 0.2, 1.0),
    Color4(0.6509803921568628, 0.33725490196078434, 0.1568627450980392, 1.0),
    Color4(0.9686274509803922, 0.5058823529411764, 0.7490196078431373, 1.0),
    Color4(0.6, 0.6, 0.6, 1.0),
    Color4(0.8941176470588236, 0.10196078431372549, 0.10980392156862745, 1.0),
    Color4(0.21568627450980393, 0.49411764705882355, 0.7215686274509804, 1.0)
)


def load_sonata(instance: Instance, path: str, population: str) -> Model:
    node_population = SonataNodePopulation(
        name=population,
        nodes=SonataNodes.all(),
        morphology=Morphology(10)
    )
    loader = SonataLoader([node_population])
    return loader.load_models(instance, path)[0]


def compute_bbp_cells(path: str, ids: list[int]) -> BbpCells:
    if len(ids) != 0:
        return BbpCells.from_gids(ids)
    circuit = bluepy.Circuit(path)
    all_ids_len = len(circuit.cells.ids())
    density = 200000.0 / all_ids_len
    density = min(density, 1.0)
    return BbpCells.from_density(density)


def load_bbp(instance: Instance, path: str, ids: list[int] = []) -> Model:
    loader = BbpLoader(
        cells=compute_bbp_cells(path, ids),
        morphology=Morphology(radius_multiplier=2, load_dendrites=True)
    )
    return loader.load_models(instance, path)[0]


def setup_scene(instance: Instance) -> None:
    clear_models(instance)
    direction = Vector3(1, -1, -1).normalized
    add_light(instance, DirectionalLight(intensity=9, direction=direction))
    add_light(instance, AmbientLight(intensity=0.25))


def snapshot(instance: Instance, bounds: Bounds, filename: str) -> None:
    ar = bounds.width / bounds.height
    resolution = Resolution(int(RESOLUTION.height * ar), RESOLUTION.height)
    camera = CameraController(bounds, ar).camera
    renderer = ProductionRenderer(128, 5, Color4.white.transparent)
    snapshot = Snapshot(resolution, camera, renderer)
    snapshot.save(instance, f"{RENDER_RESULT}/{filename}")


def render_microcircuit(instance: Instance) -> None:
    model = load_bbp(instance, MICROCIRCUIT)
    color_model(instance, model.id, CircuitColorMethod.LAYER, LAYER_COLORS_2)
    snapshot(instance, model.bounds, "microcircuit.png")


def render_thalamus(instance: Instance) -> None:
    model = load_bbp(instance, THALAMUS)
    rotation = euler(90, 0, -90, True)
    model = update_model(instance, model.id, Transform(rotation=rotation))
    color_model(instance, model.id, CircuitColorMethod.LAYER,
                THALAMUS_LAYER_COLORS)
    snapshot(instance, model.bounds, "thalamus.png")


def render_hippocampus(instance: Instance) -> None:
    model = load_bbp(instance, HIPPOCAMPUS)
    model = update_model(instance, model.id, Transform(
        rotation=euler(180, -90, 0, True)
    ))
    color_model(instance, model.id, CircuitColorMethod.LAYER,
                HIPPOCAMPUS_LAYER_COLORS)
    snapshot(instance, model.bounds, "hippocampus.png")


def render_sscx(instance: Instance) -> None:
    model = load_bbp(instance, SSCX)
    model = update_model(instance, model.id, Transform(
        rotation=euler(-90, -90, 0, True)
    ))
    color_model(instance, model.id, CircuitColorMethod.LAYER, LAYER_COLORS)
    snapshot(instance, model.bounds, "sscx.png")


def render_neocortex(instance: Instance) -> None:
    circuit = bluepy.Circuit(NCX)
    all_ids_len = len(circuit.cells.ids())
    density = 500000.0 / all_ids_len
    density = min(density, 1.0)

    layers = {
        "1": [int(id) for id in circuit.cells.ids("Layer1")],
        "2": [int(id) for id in circuit.cells.ids("Layer2")],
        "3": [int(id) for id in circuit.cells.ids("Layer3")],
        "4": [int(id) for id in circuit.cells.ids("Layer4")],
        "5": [int(id) for id in circuit.cells.ids("Layer5")],
        "6": [int(id) for id in circuit.cells.ids("Layer6")],
    }

    for layer, ids in layers.items():

        step = max(int(1 / density), 1)
        ids = [ids[i] for i in range(0, len(ids), step)]

        model = load_bbp(instance, NCX, ids)
        rotation = euler(180, -90, 0, True)
        rotation = rotation.then(euler(90, 180, 0, True))
        model = update_model(instance, model.id, Transform(rotation=rotation))
        set_model_color(instance, model.id, LAYER_COLORS[layer])

    snapshot(instance, get_bounds(instance), "neocortex.png")


def render_full_brain(instance: Instance) -> None:
    model = load_sonata(instance, FULL_BRAIN, FULL_BRAIN_POPULATION)
    model = update_model(instance, model.id, Transform(
        rotation=euler(180, 90, 0, True)))

    regions = get_color_values(instance, model.id, CircuitColorMethod.REGION)
    color_map = dict()
    for index, region in enumerate(regions):
        color_index = index % len(FREE_COLORS)
        color = FREE_COLORS[color_index]
        color_map[region] = color
    color_model(instance, model.id, CircuitColorMethod.REGION, color_map)

    bounds = model.bounds
    pos = Vector3(
        bounds.max.x,  # bounds.min.x + bounds.size.x * 0.15,
        bounds.max.y + bounds.size.y * 0.15,
        bounds.max.z - bounds.size.z * 0.15)
    target = bounds.center
    view_vector = (pos - target)
    pos = target + view_vector * 2
    view = View(pos, target)

    camera = Camera(view)
    renderer = ProductionRenderer(128, 5, Color4.white.transparent)
    snapshot = Snapshot(RESOLUTION, camera, renderer)
    snapshot.save(instance, f"{RENDER_RESULT}/fullbrain.png")


def render_human_column(instance: Instance) -> None:
    model = load_bbp(instance, HUMAN_COLUMN)
    snapshot(instance, model.bounds, "human_column.png")


def render() -> None:
    service = Service(uri="0.0.0.0:5000", executable=EXECUTABLE)
    connector = Connector(HOST)
    
    with start(service, connector) as (_, instance):

        renderables = [
            render_microcircuit,
            render_thalamus,
            render_hippocampus,
            render_sscx,
            render_neocortex,
            # render_human_column,
            render_full_brain
        ]

        for renderable in renderables:
            setup_scene(instance)
            renderable(instance)

if __name__ == "__main__":
    render()