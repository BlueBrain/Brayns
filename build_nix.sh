
#
# simple script to build nix in any bbp-nixpkgs environment
# with the nix module installed
# e.g bbpviz*, bbplxviz*, knl, jureca, theta, marconi, gallileo, cooley 
# 



#  clean the environment
module purge

# load basic dev env
module load nix/dev-env-gcc

# load openGL env
module load nix/gl-dev

# load all the viz team world
module load nix/viz/lunchbox nix/viz/zeroeq nix/viz/zerobuf nix/viz/lexis nix/viz/vmmlib nix/viz/brion 

# load intel staff
module load nix/viz/ospray nix/viz/embree

# load few extra for brayns
module load nix/jpegturbo nix/assimp

# replace hdf5 module by hdf5 with cpp bindingq
module swap nix/hdf5 nix/hdf5-cpp



## start to build
rm -rf build
mkdir -p build

cd build

# run cmake
# - disable subprojects
# - verbose mode
# - ignore -Werror ( failure with gcc 4.9.3 )
# - disable deflect 
cmake \
 -DDISABLE_SUBPROJECTS=TRUE \
 -DCOMMON_FIND_PACKAGE_QUIET=FALSE \
 -DCOMMON_DISABLE_WERROR=TRUE \
 -DBRAYNS_DEFLECT_ENABLED=OFF \
 -DBoost_NO_BOOST_CMAKE=FALSE ../ 

# build
make -j 4


