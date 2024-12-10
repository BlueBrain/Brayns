import bluepy
import json

PATH = '/gpfs/bbp.cscs.ch/project/proj55/iavarone/releases/simulations/2019-11-21_GJs_newCT/CT_flickmulti_multiseed_1550fbr/ct_flick0/seed121877/BlueConfig'
JSON = '/gpfs/bbp.cscs.ch/home/acfleury/src/Test/test.json'

circuit = bluepy.Circuit(PATH)


def load_target(target: str):
    return set(circuit.cells.ids(target).tolist())


mc2_rt = load_target('mc2;Rt')
mc2_vpl = load_target('mc2;VPL')
vpl_tc = load_target('VPL_TC') & mc2_vpl
vpl_in = load_target('VPL_IN') & mc2_vpl

rt_rc = sorted(mc2_rt)
vpl_tc = sorted(vpl_tc)
vpl_in = sorted(vpl_in)

with open(JSON, 'w') as file:
    file.write(json.dumps({
        'Rt_RC': rt_rc,
        'VPL_TC': vpl_tc,
        'VPL_IN': vpl_in
    }))
