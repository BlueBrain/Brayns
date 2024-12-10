import shutil
from pathlib import Path
from typing import NamedTuple

import bluepysnap
import brayns

URI = "r1i7n5:5000"
PATH = "/gpfs/bbp.cscs.ch/data/project/proj82/simulations/Thal-SSCx/simulation_campaigns_ctc_v3/spon_activity_sim/scan_EXC_depol/84457a2c-105a-4404-9e27-a18d5dff2590/2/simulation_config.json"

COLORS = [
    "#171786",
    "#0064BD",
    "#60099B",
    "#AC005A",
    "#DB4400",
    "#E98B00",
    "#FAF8B9",
]

SSCX_NODE_SET = "hex_71"
THALAMUS_NODE_SET = "mc2_Column"

SPIKE_TRANSITION_TIME = 100

# EDIT !!!!
FRAMES = "frames5/%d.png"

FORCE_RELOAD = False
FPS = 1
SLOWING_FACTOR = 10
TSTART = 1500
TSTOP = 3000
ZOOM = 1.1
MORPHOLOGIES = False
DENSITY = 0.05 if MORPHOLOGIES else 1.0

CELL_TO_ALIGN = 300
SSCX_ROTATION = brayns.Rotation.from_quaternion(
    brayns.Quaternion(
        -0.4775448246091835, 0.16176777871492265, 0.5858190014507291, 0.634506283497567
    )
)

RESOLUTION = brayns.Resolution.full_hd
RENDERER = brayns.InteractiveRenderer()
# RENDERER = brayns.ProductionRenderer() # Beau

COLORS_BY_LAYER = {
    "1": brayns.Color3(1.0, 0.949, 0.0),
    "2": brayns.Color3(0.968, 0.580, 0.113),
    "3": brayns.Color3(0.878, 0.184, 0.380),
    "4": brayns.Color3(0.988, 0.607, 0.992),
    "5": brayns.Color3(0.407, 0.658, 0.878),
    "6": brayns.Color3(0.423, 0.901, 0.384),
}

COLOR_BY_MTYPE = {
    "Rt_RC": brayns.Color3(1.0, 0.1, 0.1),
    "VPL_TC": brayns.Color3(0.1, 1.0, 0.1),
}


class Circuits(NamedTuple):
    sscx: brayns.Model
    thalamus: brayns.Model


def get_sscx_column_rotation() -> brayns.Rotation:
    simulation = bluepysnap.Simulation(PATH)
    circuit = simulation.circuit
    population = circuit.nodes["All"]  # type: ignore
    properties = ["orientation_x", "orientation_y", "orientation_z", "orientation_w"]

    orientations = population.get(
        group=SSCX_NODE_SET,
        properties=properties,
    )

    x = orientations[properties[0]][CELL_TO_ALIGN]
    y = orientations[properties[1]][CELL_TO_ALIGN]
    z = orientations[properties[2]][CELL_TO_ALIGN]
    w = orientations[properties[3]][CELL_TO_ALIGN]

    quaternion = brayns.Quaternion(x, y, z, w)

    return brayns.Rotation.from_quaternion(quaternion).inverse


def load_color_ramp(instance: brayns.Instance, model: brayns.Model) -> None:
    colors = brayns.ColorRamp(
        value_range=brayns.ValueRange(0, 1),
        colors=[
            brayns.Color4.from_color3(brayns.parse_hex_color(color)) for color in COLORS
        ],
    )
    brayns.set_color_ramp(instance, model.id, colors)


def color_sscx_by_layer(instance: brayns.Instance, model: brayns.Model) -> None:
    method = brayns.CircuitColorMethod.LAYER
    colors = {
        key: brayns.Color4.from_color3(color * 0.8)
        for key, color in COLORS_BY_LAYER.items()
    }

    brayns.color_model(instance, model.id, method, colors)

    brayns.enable_simulation(instance, model.id, False)


def color_thalamus_by_mtype(instance: brayns.Instance, model: brayns.Model) -> None:
    method = brayns.CircuitColorMethod.MTYPE
    colors = {
        key: brayns.Color4.from_color3(color * 0.8)
        for key, color in COLOR_BY_MTYPE.items()
    }

    brayns.color_model(instance, model.id, method, colors)

    brayns.enable_simulation(instance, model.id, False)


def move_thalamus(instance: brayns.Instance, circuits: Circuits) -> Circuits:
    sscx, thalamus = circuits

    sscx_position = sscx.bounds.center
    thalamus_position = thalamus.bounds.center
    translation = sscx_position - thalamus_position

    sscx_height = sscx.bounds.height
    translation -= sscx_height * brayns.Axis.y * 0.75

    thalamus = brayns.update_model(instance, thalamus.id, brayns.Transform(translation))

    return Circuits(sscx, thalamus)


def load_circuits(instance: brayns.Instance) -> Circuits:
    models = brayns.get_models(instance)

    if models and not FORCE_RELOAD:
        sscx = next(model for model in models if model.id == 3)
        thalamus = next(model for model in models if model.id == 4)
        return Circuits(sscx, thalamus)

    brayns.clear_models(instance)

    brayns.add_light(instance, brayns.AmbientLight(0.5))
    brayns.add_light(
        instance, brayns.DirectionalLight(5, direction=brayns.Vector3(1, -1, -1))
    )
    brayns.add_light(
        instance, brayns.DirectionalLight(2, direction=brayns.Vector3(-1, -1, -1))
    )

    report = brayns.SonataReport.spikes(SPIKE_TRANSITION_TIME)

    morphology = brayns.Morphology(
        radius_multiplier=1 if MORPHOLOGIES else 10,
        load_axon=False,
        load_dendrites=MORPHOLOGIES,
        load_soma=MORPHOLOGIES,
    )

    sscx_loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="All",
                nodes=brayns.SonataNodes.from_names([SSCX_NODE_SET], DENSITY),
                report=report,
                morphology=morphology,
            ),
        ]
    )

    sscx = sscx_loader.load_models(instance, PATH)[0]

    sscx_rotation = SSCX_ROTATION
    # sscx_rotation = get_sscx_column_rotation()

    sscx = brayns.update_model(
        instance, sscx.id, brayns.Transform(rotation=sscx_rotation)
    )

    color_sscx_by_layer(instance, sscx)

    load_color_ramp(instance, sscx)

    thalamus_loader = brayns.SonataLoader(
        [
            brayns.SonataNodePopulation(
                name="thalamus_neurons",
                nodes=brayns.SonataNodes.from_names([THALAMUS_NODE_SET], DENSITY),
                report=report,
                morphology=morphology,
            ),
        ]
    )

    thalamus = thalamus_loader.load_models(instance, PATH)[0]

    color_thalamus_by_mtype(instance, thalamus)

    load_color_ramp(instance, thalamus)

    circuits = Circuits(sscx, thalamus)

    return move_thalamus(instance, circuits)


def make_movie(instance: brayns.Instance, circuits: Circuits, pattern: str) -> None:
    resolution = brayns.Resolution.full_hd

    controller = brayns.CameraController(
        target=brayns.get_bounds(instance),
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    camera.distance *= ZOOM

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)

    for model in circuits:
        brayns.enable_simulation(instance, model.id, False)

    snapshot.save(instance, "test.png")

    for model in circuits:
        brayns.enable_simulation(instance, model.id, True)

    simulation = brayns.get_simulation(instance)

    frames = brayns.MovieFrames(
        fps=FPS,
        slowing_factor=SLOWING_FACTOR,
        start_frame=simulation.get_frame(TSTART),
        end_frame=simulation.get_frame(TSTOP),
    )

    indices = frames.get_indices(simulation)

    # ERASE OLD MOVIE FRAMES
    folder = Path(pattern).parent
    shutil.rmtree(folder, ignore_errors=True)
    folder.mkdir(parents=True, exist_ok=True)

    for i, index in enumerate(indices):
        snapshot.frame = index
        snapshot.save(instance, pattern % i)

    # Call ffmpeg
    movie = brayns.Movie(pattern, FPS)
    movie.save(str(folder / "movie.mp4"))


def main() -> None:
    connector = brayns.Connector(URI)

    with connector.connect() as instance:
        circuits = load_circuits(instance)

        make_movie(instance, circuits, FRAMES)


if __name__ == "__main__":
    main()
