import base64
import json
import math
from dataclasses import dataclass, field
from typing import List

import brayns
import numpy
from scipy.spatial.transform import Rotation

URI = 'bbpv1:5000'
PATH = '/gpfs/bbp.cscs.ch/project/proj55/iavarone/releases/simulations/2019-11-21_GJs_newCT/CT_flickmulti_multiseed_1550fbr/ct_flick0/seed121877/BlueConfig'
SNAPSHOT = '/home/acfleury/Test/simulation'
JSON = '/home/acfleury/Test/test.json'
LOAD = True
#RESOLUTION = [15360, 8640]
#RESOLUTION = [3840, 2160]
RESOLUTION = [1920, 1080]
CENTER = [-0.1, 0.13, 0.0]


@dataclass
class Params:

    angle: float = 0.0
    axis: List[float] = field(default_factory=list)
    shift: List[float] = field(default_factory=list)


PARAMS = [
    #Params(angle=45, axis=[0, 1, 0], shift=[0, -0.2, 0.4]),
    Params(angle=135, axis=[0, 1, 0], shift=[0, -0.1, 0.3]),
    #Params(angle=180, axis=[0, 1, 0], shift=[0.1, -0.28, 0.3]),
]


class ExtendedRotation:

    def __init__(self, angle: float, axis: list, origin: list):
        r = numpy.array(axis, numpy.float64)
        r /= math.sqrt(r.dot(r))
        r *= math.radians(angle)
        self._rotation = Rotation.from_rotvec(r)
        self._origin = numpy.array(origin, numpy.float64)

    @property
    def without_origin(self):
        return self._rotation

    def apply(self, point: numpy.ndarray):
        return self._origin + self._rotation.apply(point - self._origin)


@dataclass
class Scene:

    bottom_left: numpy.ndarray = field(default_factory=lambda: numpy.zeros(3))
    top_right: numpy.ndarray = field(default_factory=lambda: numpy.zeros(3))

    @staticmethod
    def load(client: brayns.Client):
        bounds = client.get_scene()['bounds']
        return Scene(
            bottom_left=numpy.array(bounds['min'], numpy.float64),
            top_right=numpy.array(bounds['max'], numpy.float64)
        )

    @property
    def center(self):
        return (self.bottom_left + self.top_right) / 2

    @property
    def size(self):
        return self.top_right - self.bottom_left

    @property
    def width(self):
        return self.size[0]

    @property
    def height(self):
        return self.size[1]

    @property
    def depth(self):
        return self.size[2]


@dataclass
class Camera:

    position: numpy.ndarray = field(default_factory=lambda: numpy.zeros(3))
    target: numpy.ndarray = field(default_factory=lambda: numpy.zeros(3))
    orientation: numpy.ndarray = field(
        default_factory=lambda: numpy.array([0, 0, 0, 1], numpy.float64)
    )
    fovy: float = 0.0

    @staticmethod
    def load(client: brayns.Client):
        camera = client.get_camera()
        return Camera(
            position=numpy.array(camera['position'], numpy.float64),
            target=numpy.array(camera['target'], numpy.float64),
            orientation=numpy.array(camera['orientation'], numpy.float64),
            fovy=math.radians(client.get_camera_params()['fovy'])
        )

    def dump(self, client: brayns.Client):
        client.set_camera(
            position=list(self.position),
            target=list(self.target),
            orientation=list(self.orientation)
        )

    def center(self, scene: Scene):
        self.position = scene.center
        self.position[2] += scene.height / 2 / math.tan(self.fovy / 2)
        self.target = scene.center

    def rotate(self, rotation: Rotation):
        self.orientation = (
            rotation * Rotation.from_quat(self.orientation)
        ).as_quat()


GIDS = []
with open(JSON) as file:
    GIDS = json.load(file)


def load_circuits(explorer: brayns.CircuitExplorer):
    explorer.load_circuit(
        circuit_color_scheme=explorer.CIRCUIT_COLOR_SCHEME_NEURON_BY_MTYPE,
        path=PATH,
        targets=['mc2;Rt'],
        gids=GIDS['Rt_RC'][::20],
        use_sdf=False,
        load_axon=False,
        load_soma=False,
        load_dendrite=True,
        load_apical_dendrite=False,
        radius_multiplier=3
    )
    explorer.load_circuit(
        circuit_color_scheme=explorer.CIRCUIT_COLOR_SCHEME_NEURON_BY_MTYPE,
        path=PATH,
        targets=['mc2;Rt'],
        gids=GIDS['Rt_RC'][400:401],
        use_sdf=False,
        load_axon=False,
        load_soma=False,
        load_dendrite=True,
        load_apical_dendrite=False,
        radius_multiplier=3
    )
    explorer.load_circuit(
        circuit_color_scheme=explorer.CIRCUIT_COLOR_SCHEME_NEURON_BY_MTYPE,
        path=PATH,
        targets=['mc2;Rt'],
        gids=GIDS['Rt_RC'][400:401],
        use_sdf=False,
        load_axon=True,
        load_soma=False,
        load_dendrite=False,
        load_apical_dendrite=False,
        radius_multiplier=32
    )
    explorer.load_circuit(
        circuit_color_scheme=explorer.CIRCUIT_COLOR_SCHEME_NEURON_BY_MTYPE,
        path=PATH,
        targets=['VPL_TC'],
        gids=GIDS['VPL_TC'][10::150],
        use_sdf=False,
        load_axon=False,
        load_dendrite=True,
        load_apical_dendrite=False,
        radius_multiplier=2,
        dampen_branch_thickness_changerate=False
    )
    explorer.load_circuit(
        circuit_color_scheme=explorer.CIRCUIT_COLOR_SCHEME_NEURON_BY_MTYPE,
        path=PATH,
        targets=['VPL_IN'],
        gids=GIDS['VPL_IN'],
        use_sdf=False,
        load_axon=False,
        load_dendrite=True,
        load_apical_dendrite=False,
        radius_multiplier=2,
        dampen_branch_thickness_changerate=False
    )


def color_hex_to_list(hex: str):
    return [
        int(hex[2*i:2*i+2], base=16) / 255
        for i in range(3)
    ]


def set_color(explorer: brayns.CircuitExplorer, model_id: int, color: str):
    explorer.set_material_range(
        model_id,
        material_ids=[],
        diffuse_color=color_hex_to_list(color),
        specular_color=(1.0, 1.0, 1.0),
        specular_exponent=20.0,
        opacity=1.0,
        reflection_index=0.0,
        refraction_index=1.0,
        simulation_data_cast=True,
        glossiness=1.0,
        shading_mode=brayns.CircuitExplorer.SHADING_MODE_NONE,
        emission=0.0,
        clipping_mode=brayns.CircuitExplorer.CLIPPING_MODE_NONE,
        user_parameter=0.0
    )


def color_models(explorer: brayns.CircuitExplorer):
    set_color(explorer, 0, '1f77b4')
    set_color(explorer, 1, 'ff0000')
    set_color(explorer, 2, 'ff0000')
    set_color(explorer, 3, 'ff7f02')
    set_color(explorer, 4, '2ca02c')


def prepare_camera(client: brayns.Client, shift: numpy.ndarray):
    client.set_camera(current='perspective')
    scene = Scene.load(client)
    camera = Camera.load(client)
    camera.center(scene)
    camera.position += shift * scene.size
    camera.orientation = numpy.array([0, 0, 0, 1], numpy.float64)
    camera.dump(client)
    return camera


def adjust_renderer(client: brayns.Client):
    client.set_renderer(
        head_light=False,
        background_color=[1, 1, 1]
    )
    client.set_renderer_params({
        'epsilonFactor': 1,
        'exposure': 1,
        'fogStart': 0,
        'fogThickness': 100000000,
        'giDistance': 10000,
        'giSamples': 0,
        'giWeight': 0,
        'maxBounces': 5,
        'maxDistanceToSecondaryModel': 30,
        'samplingThreshold': 0.001,
        'shadows': 1,
        'softShadows': 0.4,
        'softShadowsSamples': 1,
        'volumeAlphaCorrection': 0.5,
        'volumeSpecularExponent': 20
    })


def adjust_light(client: brayns.Client, direction: list):
    client.clear_lights()
    client.add_light_directional(
        color=[1, 1, 1],
        intensity=4,
        is_visible=True,
        direction=direction,
        angular_diameter=1
    )


def take_snapshot(client: brayns.Client, name: str):
    result = client.snapshot(
        format='png',
        size=RESOLUTION,
        samples_per_pixel=128
    )
    with open(f'{SNAPSHOT}/{name}.png', 'wb') as file:
        file.write(base64.b64decode(result['data']))


def render(client: brayns.Client, angle: float, axis: list, shift: list):
    camera = prepare_camera(client, numpy.array(CENTER))
    scene = Scene.load(client)
    rotation = ExtendedRotation(angle, axis, list(scene.center))
    adjust_light(
        client,
        list(rotation.without_origin.apply(numpy.array([-1, -1, 0])))
    )
    camera.position += numpy.array(shift * scene.size, numpy.float64)
    camera.position = rotation.apply(camera.position)
    camera.rotate(rotation.without_origin)
    camera.dump(client)
    take_snapshot(
        client,
        f'image_{angle}deg_{"".join(str(i) for i in axis)}'
    )


with brayns.connect(URI) as client:
    explorer = brayns.CircuitExplorer(client)
    if LOAD:
        load_circuits(explorer)
    color_models(explorer)
    adjust_renderer(client)
    for params in PARAMS:
        render(client, params.angle, params.axis, params.shift)
