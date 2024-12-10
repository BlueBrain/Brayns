import json
import sys
import traceback

import numpy as np
from bluepy import Circuit
from brayns import CircuitExplorer, Client


def add_conepills(
    start_list,
    orientation_list,
    radius1=25,
    radius2=1,
    color=[1, 0, 0, 1],
    name_prefix="arrow_",
    multiplier=500,
):
    for i in range(len(start_list)):
        st = list(start_list.iloc[i].values)
        end = list(st + multiplier * orientation_list[i])
        circuitExplorer.add_conepill(
            st, end, radius1, radius2, color, name=name_prefix + str(i)
        )


def api(name, params=None):
    try:
        print(">", name)
        return braynsClient.rockets_client.request(name, params)
    except Exception as ex:
        print()
        print("########## ERROR ##########")
        print("Entrypoint: ", name)
        print("Params: ", json.dumps(params, indent=4))
        print()
        print(traceback.format_exc())
        print()
        sys.exit(2)


def as_str(my_list):
    s = ""
    for i in my_list:
        s += str(i)
        s += ","
    return s[:-1]  # ignore last comma


print("Loading Client")
braynsClient = Client(
    "r1i7n31.bbp.epfl.ch:26502"
)  # edit node name and port from the allocated brayns
# e.g.
# http://webbrayns.ocp.bbp.epfl.ch/?host=r1i7n31.bbp.epfl.ch%3A26943%0A&account=proj112&partition=prod&allocator=unicore
# before .bbp.epfl part and port no after 3A part
circuitExplorer = CircuitExplorer(braynsClient)

print("Loading Circuit")
PATH_TO_CIRCUIT = "/gpfs/bbp.cscs.ch/project/proj112/scratch/circuits/20211110-BioM/"
config_type = "CircuitConfig_struct"
c = Circuit(PATH_TO_CIRCUIT + config_type)

print("Calculating positions and orientations")
np.random.seed(10)
size = 15
rand_sppc_gids = np.random.choice(c.cells.get("SP_PC").index, size)
rand_sppc_pos = c.cells.positions(rand_sppc_gids)
transverse_ori = [
    c.cells.orientations(rand_sppc_gids).values[i][:, 0] for i in range(size)
]

# Add ca1 mesh using proj112/entities/atlas/20211004_BioM/meshes/SP.stl

path = "/gpfs/bbp.cscs.ch/project/proj112/entities/atlas/20211004_BioM/meshes/SP.stl"
loader_name = "mesh"
loader_properties = {"geometryQuality": 2}

print("Plotting mesh")
data = braynsClient.add_model(
    path=path, loader_name=loader_name, loader_properties=loader_properties
)


circuitExplorer.set_material_range(
    model_id=0,
    material_ids=[],
    diffuse_color=[90, 0, 0],
    specular_color=[1, 1, 1],
    specular_exponent=10.0,
    opacity=0.6,
    reflection_index=0,
    refraction_index=1,
    simulation_data_cast=False,
    glossiness=1.0,
    shading_mode=circuitExplorer.SHADING_MODE_NONE,
    clipping_mode=circuitExplorer.CLIPPING_MODE_NONE,
)


print("Plotting orientations")
# Adds orientation conepills, choose rgba color or tune from brayns later
add_conepills(rand_sppc_pos, transverse_ori, color=[0, 230, 64, 1])

print("Plotting axons")
# Adds axons of selected gids
_MODEL_ = api(
    "add-model",
    {
        "bounding_box": False,
        "loader_name": "Advanced circuit loader (Experimental)",
        "loader_properties": {
            "000_db_connection_string": "",
            "001_density": 0.01,
            "002_random_seed": 0,
            "010_targets": "",
            "011_gids": as_str(rand_sppc_gids),
            "020_report": "",
            "021_report_type": "Undefined",
            "022_user_data_type": "Undefined",
            "023_synchronous_mode": True,
            "024_spike_transition_time": 1,
            "030_circuit_color_scheme": "By id",
            "040_mesh_folder": "",
            "041_mesh_filename_pattern": "mesh_{gid}.obj",
            "042_mesh_transformation": False,
            "050_radius_multiplier": 10,
            "051_radius_correction": 15,
            "052_section_type_soma": True,
            "053_section_type_axon": True,
            "054_section_type_dendrite": False,
            "055_section_type_apical_dendrite": False,
            "060_use_sdfgeometry": True,
            "061_dampen_branch_thickness_changerate": True,
            "070_realistic_soma": False,
            "071_metaballs_samples_from_soma": 5,
            "072_metaballs_grid_size": 20,
            "073_metaballs_threshold": 1,
            "080_morphology_color_scheme": "None",
            "090_morphology_quality": "High",
            "091_max_distance_to_soma": 1.7976931348623157e308,
            "100_cell_clipping": False,
            "101_areas_of_interest": 0,
            "110_synapse_radius": 1,
            "111_load_afferent_synapses": False,
            "112_load_efferent_synapses": False,
            "120_load_layers": True,
            "121_load_etypes": True,
            "122_load_mtypes": True,
        },
        "name": "Axons",
        "path": PATH_TO_CIRCUIT + config_type,
        "visible": True,
    },
)
