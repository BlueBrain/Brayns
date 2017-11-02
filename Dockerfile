# Docker container for running Brayns as a service
# Check https://docs.docker.com/engine/userguide/eng-image/dockerfile_best-practices/#user for best practices.

# This Dockerfile leverages multi-stage builds, available since Docker 17.05
# See: https://docs.docker.com/engine/userguide/eng-image/multistage-build/#use-multi-stage-builds

# Image where Brayns is built
FROM ubuntu:xenial as builder
LABEL maintainer="bbp-svc-viz@groupes.epfl.ch"
ARG DIST_PATH=/app/dist

# Get ISPC
# https://ispc.github.io/downloads.html
ARG ISPC_VERSION=1.9.1
ARG ISPC_DIR=ispc-v${ISPC_VERSION}-linux
ARG ISPC_PATH=/app/$ISPC_DIR

RUN mkdir -p ${ISPC_PATH} \
 && apt-get update \
 && apt-get -y install wget \
 && wget http://netix.dl.sourceforge.net/project/ispcmirror/v${ISPC_VERSION}/${ISPC_DIR}.tar.gz \
 && tar zxvf ${ISPC_DIR}.tar.gz -C ${ISPC_PATH} --strip-components=1 \
 && rm -rf ${ISPC_PATH}/${ISPC_DIR}/examples

# Add ispc bin to the PATH
ENV PATH $PATH:${ISPC_PATH}

# Install Embree
# https://github.com/embree/embree
ARG EMBREE_VERSION=2.17.0
ARG EMBREE_SRC=/app/embree

RUN mkdir -p ${EMBREE_SRC} \
 && apt-get -y install \
    build-essential \
    cmake \
    freeglut3-dev \
    git \
    libtbb-dev \
    libxi-dev \
    libxmu-dev \
    ninja-build \
 && git clone https://github.com/embree/embree.git ${EMBREE_SRC} \
 && cd ${EMBREE_SRC} \
 && git checkout v${EMBREE_VERSION} \
 && mkdir -p build \
 && cd build \
 && cmake .. -GNinja -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
 && ninja install

# Install OSPray
# https://github.com/ospray/OSPRay
ARG OSPRAY_VERSION=1.4.0
ARG OSPRAY_SRC=/app/ospray

RUN mkdir -p ${OSPRAY_SRC} \
 && apt-get update \
 && apt-get -y install \
    freeglut3-dev \
    libglu1-mesa-dev \
    libtbb-dev \
    libxi-dev \
    libxmu-dev \
    xorg-dev \
 && git clone https://github.com/ospray/ospray.git ${OSPRAY_SRC} \
 && cd ${OSPRAY_SRC} \
 && git checkout v${OSPRAY_VERSION} \
 && mkdir -p build \
 && cd build \
 && cmake .. -GNinja -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
 && ninja install

# Set working dir and copy Brayns assets
ARG BRAYNS_SRC=/app/brayns
WORKDIR /app
ADD . ${BRAYNS_SRC}


# Install Brayns
# https://github.com/BlueBrain/Brayns
RUN cksum ${BRAYNS_SRC}/.gitsubprojects \
 && cd ${BRAYNS_SRC} \
 && apt-get -y install \
    freeglut3-dev \
    libassimp-dev \
    libboost-all-dev \
    libglew-dev \
    libglu1-mesa-dev \
    libhdf5-serial-dev \
    libjpeg-turbo8-dev \
    libmagick++-dev \
    libtbb-dev \
    libturbojpeg \
    libxi-dev \
    libxmu-dev \
    libzmq3-dev \
    python-pyparsing \
    qtbase5-dev \
    qtdeclarative5-dev \
    xorg-dev \
 && git submodule update --init --recursive --remote \
 && mkdir -p build \
 && cd build \
 && cmake .. -GNinja \
    -DBRAYNS_BRION_ENABLED=ON \
    -DBRAYNS_DEFLECT_ENABLED=ON \
    -DBRAYNS_NETWORKING_ENABLED=ON \
    -DCLONE_SUBPROJECTS=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
 && ninja install

# Final image, containing only Brayns and libraries required to run it
FROM ubuntu:xenial
ARG DIST_PATH=/app/dist

RUN apt-get update \
 && apt-get install -y \
    freeglut3 \
    libassimp3v5 \
    libboost-atomic1.58.0 \
    libboost-chrono1.58.0 \
    libboost-date-time1.58.0 \
    libboost-filesystem1.58.0 \
    libboost-program-options1.58.0 \
    libboost-regex1.58.0 \
    libboost-serialization1.58.0 \
    libboost-system1.58.0 \
    libboost-thread1.58.0 \
    libglew1.13 \
    libgomp1 \
    libhdf5-10 \
    libhdf5-cpp-11 \
    libhdf5-cpp-11 \
    libhwloc5 \
    libmagick++-6.q16-5v5 \
    libmagickwand-6.q16-2 \
    libqt5concurrent5 \
    libqt5network5 \
    libtbb2 \
    libturbojpeg \
    libxmu6 \
    libzmq5 \
 && rm -rf /var/lib/apt/lists/*

# The COPY command below will:
# 1. create a container based on the `builder` image (but do not start it)
#    Equivalent to the `docker create` command
# 2. create a new image layer containing the
#    /app/dist directory of this new container
#    Equivalent to the `docker copy` command.
COPY --from=builder ${DIST_PATH} ${DIST_PATH}

# Add binaries from dist to the PATH
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:${DIST_PATH}/lib
ENV PATH ${DIST_PATH}/bin:$PATH

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
