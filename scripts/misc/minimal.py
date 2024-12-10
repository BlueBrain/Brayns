import logging

import brayns

EXECUTABLE = "/home/acfleury/Source/repos/Brayns/build/bin/braynsService"
OSPRAY = "/home/acfleury/Source/repos/OSPRAY_2_10/install_dir/lib"
PATH = "/gpfs/bbp.cscs.ch/project/proj3/TestData/install/share/BBPTestData/circuitBuilding_1000neurons/BlueConfig"

service = brayns.Service(
    "localhost:5000",
    executable=EXECUTABLE,
    env={"LD_LIBRARY_PATH": OSPRAY},
)

connector = brayns.Connector(
    "localhost:5000",
    logger=brayns.Logger(logging.INFO),
    max_attempts=None,
)

with brayns.start(service, connector) as (process, instance):

    loader = brayns.BbpLoader()
    models = loader.load_models(instance, PATH)

    ids = brayns.get_circuit_ids(instance, models[0].id)

    print(ids)
