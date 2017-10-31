# Docker container for running Brayns as a service
# Check https://docs.docker.com/engine/userguide/eng-image/dockerfile_best-practices/#user for best practices.
FROM ubuntu:xenial

ENV DIST_PATH /app/dist
ENV BUILD_TOOLS build-essential cmake ninja-build git


# Get ISPC
# https://ispc.github.io/downloads.html
ENV ISPC_VERSION 1.9.1
ENV ISPC_DIR ispc-v${ISPC_VERSION}-linux
ENV ISPC_PATH /app/$ISPC_DIR

RUN mkdir -p ${ISPC_PATH} && \
    apt-get update && \
    apt-get -y install wget && \
    wget http://netix.dl.sourceforge.net/project/ispcmirror/v${ISPC_VERSION}/${ISPC_DIR}.tar.gz && \
    tar zxvf ${ISPC_DIR}.tar.gz -C ${ISPC_PATH} --strip-components=1 && \
    rm $ISPC_DIR.tar.gz && \
    apt-get -y remove wget && \
    apt-get -y autoremove && \
    apt-get clean

# Add ispc bin to the PATH
ENV PATH $PATH:${ISPC_PATH}


# Install Embree
# https://github.com/embree/embree
ENV EMBREE_VERSION 2.17.0
ENV EMBREE_SRC /app/embree

RUN mkdir -p ${EMBREE_SRC} && \
    apt-get update && \
    apt-get -y install ${BUILD_TOOLS} && \
    apt-get -y install freeglut3-dev \
    libtbb-dev \
    libxi-dev \
    libxmu-dev && \
    git clone https://github.com/embree/embree.git ${EMBREE_SRC} && \
    cd ${EMBREE_SRC} && \
    git checkout v${EMBREE_VERSION} && \
    mkdir -p build && \
    cd build && \
    cmake .. -GNinja -DCMAKE_INSTALL_PREFIX=${DIST_PATH} && \
    ninja install && \
    cd /app && \
    rm -rf ${EMBREE_SRC} && \
    apt-get -y remove ${BUILD_TOOLS} && \
    apt-get -y remove freeglut3-dev libtbb-dev libxi-dev libxmu-dev && \
    apt-get -y autoremove && \
    apt-get clean


# Install OSPray
# https://github.com/ospray/OSPRay
ENV OSPRAY_VERSION 1.4.0
ENV OSPRAY_SRC /app/ospray

RUN mkdir -p ${OSPRAY_SRC} && \
    apt-get update && \
    apt-get -y install ${BUILD_TOOLS} && \
    apt-get -y install freeglut3-dev \
    libglu1-mesa-dev \
    libtbb-dev \
    libxi-dev \
    libxmu-dev \
    xorg-dev && \
    git clone https://github.com/ospray/ospray.git ${OSPRAY_SRC} && \
    cd ${OSPRAY_SRC} && \
    git checkout v${OSPRAY_VERSION} && \
    mkdir -p build && \
    cd build && \
    cmake .. -GNinja -DCMAKE_INSTALL_PREFIX=${DIST_PATH} && \
    ninja install && \
    cd /app && \
    rm -rf ${OSPRAY_SRC} && \
    apt-get -y remove ${BUILD_TOOLS} && \
    apt-get -y remove freeglut3-dev libglu1-mesa-dev libtbb-dev libxi-dev libxmu-dev xorg-dev && \
    apt-get -y autoremove && \
    apt-get clean


# Set working dir and copy Brayns assets
ENV BRAYNS_SRC /app/brayns
WORKDIR /app
ADD . ${BRAYNS_SRC}


# Install Brayns
# https://github.com/BlueBrain/Brayns
RUN cksum ${BRAYNS_SRC}/.gitsubprojects && \
    cd ${BRAYNS_SRC} && \
    apt-get update && \
    apt-get -y install ${BUILD_TOOLS} && \
    apt-get -y install libtbb-dev \
    freeglut3-dev \
    libassimp-dev \
    libboost-all-dev \
    libglew-dev \
    libglu1-mesa-dev \
    libhdf5-serial-dev \
    libjpeg-turbo8-dev \
    libmagick++-dev \
    libturbojpeg \
    libxi-dev \
    libxmu-dev \
    libzmq3-dev \
    python-pyparsing \
    qtbase5-dev \
    qtdeclarative5-dev \
    xorg-dev && \
    git submodule update --init --recursive --remote && \
    mkdir -p build && \
    cd build && \
    cmake .. -GNinja \
    -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
    -DCMAKE_BUILD_TYPE=Release \
    -DBRAYNS_NETWORKING_ENABLED=ON \
    -DBRAYNS_DEFLECT_ENABLED=ON \
    -DBRAYNS_BRION_ENABLED=ON \
    -DCLONE_SUBPROJECTS=ON && \
    ninja install && \
    cd /app && \
    rm -rf ${BRAYNS_SRC}/build && \
    apt-get -y remove ${BUILD_TOOLS} && \
    apt-get -y purge *-dev python-pyparsing && \
    apt-get clean


# Add binaries from dist to the PATH
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:${DIST_PATH}/lib
ENV PATH $PATH:${DIST_PATH}/bin


# Expose a port from the container
# For more ports, use the `--expose` flag when running the container,
# see https://docs.docker.com/engine/reference/run/#expose-incoming-ports for docs.
EXPOSE 8200

# When running `docker run -ti --rm -p 8200:8200 brayns`,
# this will be the cmd that will be executed (+ the CLI options from CMD).
# To ssh into the container (or override the default entry) use:
# `docker run -ti --rm --entrypoint bash -p 8200:8200 brayns`
# See https://docs.docker.com/engine/reference/run/#entrypoint-default-command-to-execute-at-runtime
# for more docs
ENTRYPOINT ["braynsService"]
CMD ["--zeroeq-http-server", ":8200"]
