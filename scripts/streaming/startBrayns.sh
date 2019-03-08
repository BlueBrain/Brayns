#! /bin/bash

bitrate=15 # Mbps
fps=30     # target FPS of Brayns and the stream. If Brayns renders faster, it just waits
spp=1      # Samples per pixel for rendering

# ratio of the resolution to make it smaller from the target OpenDeck resolution, accepts floats as well.
# Note that this might mess up the positioning and size of the video players, adjust their geometry then.
ratio=1

#scene=/gpfs/bbp.cscs.ch/home/bbpvizsoa/demos/tracking_test3.obj
#scene=/gpfs/bbp.cscs.ch/project/proj3/demos/OpenDeck.obj
#scene=demo
#scene=/gpfs/bbp.cscs.ch/project/proj3/demos/ResidentEvil/Monsters.obj
#scene=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/PBRTestScene/PBRTestScene.obj
#scene=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/OpenDeckGrids/OpenDeckGrids.obj
#scene=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/OpenDeckForest/Forest.obj
#scene=/gpfs/bbp.cscs.ch/project/proj3/demos/CheckerScene/checker.obj
#scene="/gpfs/bbp.cscs.ch/project/proj3/artifacts/26.06.2019-astrocyte-for-opendeck/astrocyte-2-512.ply"
#scene="/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/3d-neuronen.obj"
#scene=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/NeuronScene/Neuron.obj
#scene=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/NeuronScene/Vascu.obj
#scene=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/NeuronScene/smallNeurons.obj
#scene=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/MegaScene/MegaScene.obj
#scene=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/AstrocyteScene/astrocyte_membrane.obj
scene=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/AstrocyteSceneFinal/astrocyteScene.obj

#envmap=/gpfs/bbp.cscs.ch/project/proj3/resources/envmap/newport_loft/newport_loft.hdr
#envmap=/gpfs/bbp.cscs.ch/project/proj3/resources/envmap/HDR_Free_City_Night_Lights/HDR_Free_City_Night_Lights_Ref.hdr
#envmap=/gpfs/bbp.cscs.ch/project/proj3/resources/envmap/HDR_029_Sky_Cloudy_Free/HDR_029_Sky_Cloudy_Ref.hdr
#envmap=/gpfs/bbp.cscs.ch/project/proj3/resources/envmap/Night_Free/night_free_Ref.hdr
#envmap=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/NeuronScene/textures/SPACE-1.hdr
#envmap=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/MegaScene/hdr/kiara_3_morning_8k.hdr
envmap=/gpfs/bbp.cscs.ch/project/proj3/resources/meshes/AstrocyteSceneFinal/textures/envMapSpace02.hdr

#args=()
args=(--renderer bbp --env-map $envmap)

###############################################################################

export CUDA_VISIBLE_DEVICES=$(($SLURM_PROCID%4))

host=$1
width=$(echo "scale=0; $2/$ratio" | bc)
height=$(echo "scale=0; $3/$ratio" | bc)
segment=$4

# hpe-mpi srun needs all ranks do the same thing, vrpn loaded last only on master would work too, but adds latency of 1 frame...
vrpn_plugin=
if [ $SLURM_PROCID == 0 ]
then
    vrpn_plugin=(--plugin "braynsVRPN")
else
    vrpn_plugin=(--plugin "braynsVRPN bla")
fi

# After how many frames an I-frame shall be sent.
# Need I-frames to compensate for packet loss to let players catch up again.
gop=$(($fps*2))
streamer_args="--stats --master-stats --async-encode"

PATH_TO_BRAYNS=/gpfs/bbp.cscs.ch/home/nachbaur/Brayns/build_spack/bin
$PATH_TO_BRAYNS/braynsRunner --engine optix "${vrpn_plugin[@]}" --plugin "braynsStreamer --segment $segment --gop $gop --fps $fps --bitrate $bitrate --host $host --width $width --height $height --mpi ${streamer_args[@]}" $scene --samples-per-pixel $spp "${args[@]}" --disable-accumulation --no-head-light --camera opendeck --window-size $width $height
