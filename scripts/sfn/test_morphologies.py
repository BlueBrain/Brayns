import brayns

PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/FULL_BRAIN_WITH_SIM_15_06_2023/morphologies/cylindrical_morphology_20.5398.swc"
PATH = "/gpfs/bbp.cscs.ch/home/acfleury/src/Test/fullbrain/test.asc"
PATH = "/gpfs/bbp.cscs.ch/project/proj3/cloned_circuits/GeneratedTest/morphologies/cerebellum_neurons/synthesized/e3e70682c2094cac629f6fbed82c07cd.asc"
PATH = "/gpfs/bbp.cscs.ch/project/proj112/entities/morphologies/20211019/ascii/dend-oh140807_A0_idJ_axon-mpg141017_a1-2_idC.asc"
PATH = "/gpfs/bbp.cscs.ch/project/proj112/entities/morphologies/20211019/ascii/mpg141017_a1-2_idC_-_Clone_9.asc"
PATH = "/gpfs/bbp.cscs.ch/project/proj112/entities/morphologies/20211019/ascii/mpg141017_a1-2_idC_-_Scale_x1.000_y1.100_z1.000.asc"
PATH = "/gpfs/bbp.cscs.ch/data/scratch/proj134/fullcircuit/full-synth-more-split-v2/CUL4_5mo_and_friends.h5/morphologies/hashed/e3/e3e70682c2094cac629f6fbed82c07cd.h5"

URI = "r2i2n0:5000"

connector = brayns.Connector(URI)

with connector.connect() as instance:
    brayns.clear_models(instance)

    brayns.add_light(instance, brayns.AmbientLight(1))

    morphology = brayns.Morphology(
        load_soma=True,
        load_dendrites=True,
    )
    loader = brayns.MorphologyLoader(morphology)

    model = loader.load_models(instance, PATH)[0]

    resolution = brayns.Resolution.ultra_hd

    controller = brayns.CameraController(
        target=model.bounds,
        aspect_ratio=resolution.aspect_ratio,
    )
    camera = controller.camera

    renderer = brayns.InteractiveRenderer()

    snapshot = brayns.Snapshot(resolution, camera, renderer)
    snapshot.save(instance, "test.png")
