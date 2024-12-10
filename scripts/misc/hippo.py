import json
import logging

import brayns

URI = 'localhost:5000'
PATH = '/gpfs/bbp.cscs.ch/project/proj112/circuits/CA1/20211110-BioM/CircuitConfig'
OUTPUT = '/gpfs/bbp.cscs.ch/home/acfleury/src/Test/frames'


with brayns.connect(URI, log_level=logging.DEBUG) as instance:

    loader = brayns.BbpLoader(
        cells=brayns.BbpCells.from_density(0.01),
        load_dendrites=True
    )
    model = loader.load(instance, PATH)[0]

    model = brayns.Model.from_instance(instance, 0)

    euler = brayns.Vector3(28.02, 50.447, 171.166)
    orientation = brayns.Quaternion.from_euler(euler, degrees=True)
    model.transform.rotation = orientation
    model = brayns.Model.update(instance, model.id, transform=model.transform)

    gids = list[int]()
    with open('gids.json') as file:
        gids = json.load(file)

    COLORS = [
        [1.0, 0.9490196078431372, 0.0],
        [0.9686274509803922, 0.5803921568627451, 0.11372549019607843],
        [0.8784313725490196, 0.1843137254901961, 0.3803921568627451],
        [0.9882352941176471, 0.6078431372549019, 0.9921568627450981],
        [0.40784313725490196, 0.6588235294117647, 0.8784313725490196],
        [0.4235294117647059, 0.9019607843137255, 0.3843137254901961],
        [0.6509803921568628, 0.33725490196078434, 0.1568627450980392],
        [0.9686274509803922, 0.5058823529411764, 0.7490196078431373],
        [0.6, 0.6, 0.6],
        [0.8941176470588236, 0.10196078431372549, 0.10980392156862745],
        [0.21568627450980393, 0.49411764705882355, 0.7215686274509804],
        [0.30196078431372547, 0.6862745098039216, 0.2901960784313726],
        [0.596078431372549, 0.3058823529411765, 0.6392156862745098],
        [1.0, 0.4980392156862745, 0.0],
        [1.0, 1.0, 0.2],
        [0.6509803921568628, 0.33725490196078434, 0.1568627450980392],
        [0.9686274509803922, 0.5058823529411764, 0.7490196078431373],
        [0.6, 0.6, 0.6],
        [0.8941176470588236, 0.10196078431372549, 0.10980392156862745],
        [0.21568627450980393, 0.49411764705882355, 0.7215686274509804]
    ]

    def random_colors():
        i = 0
        while True:
            yield brayns.Color(*COLORS[i])
            i = (i + 1) % len(COLORS)

    colors = {
        brayns.CellId.from_integer(gid): value
        for gid, value in zip(gids, random_colors())
    }

    color = brayns.CircuitColorById(colors)
    not_colored = color.apply(instance, model.id)

    '''with open('gids.json', 'w') as file:
        json.dump(not_colored, file)'''

    camera = brayns.PerspectiveCamera()
    view = camera.get_full_screen_view(model.bounds)

    euler = brayns.Vector3(-7.538, 9, 0)
    rotation = brayns.Quaternion.from_euler(euler, degrees=True)

    view.position = rotation.rotate(view.position, view.target)

    brayns.Light.clear(instance)
    light = brayns.DirectionalLight(
        intensity=2,
        direction=view.target - view.position
    )
    light.add(instance)

    renderer = brayns.ProductionRenderer.default()

    snapshot = brayns.Snapshot(
        resolution=brayns.Resolution.production,
        view=view,
        camera=camera,
        renderer=renderer
    )
    snapshot.save(instance, 'snapshot.png')
