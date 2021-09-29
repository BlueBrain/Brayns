# Docker container for running Brayns as a service
# Check https://docs.docker.com/engine/userguide/eng-image/dockerfile_best-practices/#user for best practices.

# This Dockerfile leverages multi-stage builds, available since Docker 17.05
# See: https://docs.docker.com/engine/userguide/eng-image/multistage-build/#use-multi-stage-builds

# Image where Brayns is built
FROM debian:buster-slim as builder
LABEL maintainer="bbp-svc-viz@groupes.epfl.ch"
ARG DIST_PATH=/app/dist

# Install packages
RUN apt-get update \
   && apt-get -y --no-install-recommends install \
   build-essential \
   cmake \
   git \
   ninja-build \
   libarchive-dev \
   libassimp-dev \
   libfreeimage-dev \
   libhdf5-serial-dev \
   libtbb-dev \
   libturbojpeg0-dev \
   pkg-config \
   wget \
   ca-certificates \
   libssl-dev \
   && apt-get clean \
   && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Get ISPC
# https://ispc.github.io/downloads.html

ARG ISPC_VERSION=1.10.0b
ARG ISPC_DIR=ispc-v${ISPC_VERSION}-linux
ARG ISPC_PATH=/app/ispc-v1.10.0-linux

RUN mkdir -p ${ISPC_PATH} \
   && wget https://github.com/ispc/ispc/releases/download/v1.10.0/${ISPC_DIR}.tar.gz \
   && tar zxvf ${ISPC_DIR}.tar.gz -C ${ISPC_PATH} --strip-components=1 \
   && rm -rf ${ISPC_PATH}/${ISPC_DIR}/examples

# Add ispc bin to the PATH
ENV PATH $PATH:${ISPC_PATH}

# Install embree
# https://github.com/embree/embree/releases
ARG EMBREE_VERSION=3.5.2
ARG EMBREE_FILE=embree-${EMBREE_VERSION}.x86_64.linux.tar.gz
RUN mkdir -p ${DIST_PATH} \
   && wget https://github.com/embree/embree/releases/download/v${EMBREE_VERSION}/${EMBREE_FILE} \
   && tar zxvf ${EMBREE_FILE} -C ${DIST_PATH} --strip-components=1 \
   && rm -rf ${DIST_PATH}/bin ${DIST_PATH}/doc

# Install OSPRay
# https://github.com/ospray/ospray/releases
ARG OSPRAY_TAG=v1.8.5
ARG OSPRAY_SRC=/app/ospray

RUN mkdir -p ${OSPRAY_SRC} \
   && git clone https://github.com/ospray/ospray.git ${OSPRAY_SRC} \
   && cd ${OSPRAY_SRC} \
   && git checkout ${OSPRAY_TAG} \
   && mkdir -p build \
   && cd build \
   && CMAKE_PREFIX_PATH=${DIST_PATH} cmake .. -GNinja \
   -DOSPRAY_ENABLE_TUTORIALS=OFF \
   -DOSPRAY_ENABLE_APPS=OFF \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
   && ninja install

# Install libwebsockets (2.0 from Debian is not reliable)
# https://github.com/warmcat/libwebsockets/releases
ARG LWS_VERSION=2.3.0
ARG LWS_SRC=/app/libwebsockets
ARG LWS_FILE=v${LWS_VERSION}.tar.gz

RUN mkdir -p ${LWS_SRC} \
   && wget https://github.com/warmcat/libwebsockets/archive/${LWS_FILE} \
   && tar zxvf ${LWS_FILE} -C ${LWS_SRC} --strip-components=1 \
   && cd ${LWS_SRC} \
   && mkdir -p build \
   && cd build \
   && cmake .. -GNinja \
   -DCMAKE_BUILD_TYPE=Release \
   -DLWS_STATIC_PIC=ON \
   -DLWS_WITH_SSL=OFF \
   -DLWS_WITH_ZLIB=OFF \
   -DLWS_WITH_ZIP_FOPS=OFF \
   -DLWS_WITHOUT_EXTENSIONS=ON \
   -DLWS_WITHOUT_TESTAPPS=ON \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
   && ninja install

ARG BOOST_SRC=/app/boost

RUN mkdir ${BOOST_SRC} \
   && cd ${BOOST_SRC}  \
   && wget https://boostorg.jfrog.io/artifactory/main/release/1.70.0/source/boost_1_70_0.tar.gz \
   && tar -xzf boost_1_70_0.tar.gz  \
   && cd ./boost_1_70_0  \
   &&  ./bootstrap.sh  \
   && ./b2 install -j 10

RUN wget https://github.com/Kitware/CMake/releases/download/v3.18.5/cmake-3.18.5.tar.gz \
   && tar xf cmake-3.18.5.tar.gz \
   && cd cmake-3.18.5 \
   && ./configure \
   && make -j 10 install

# Set working dir and copy Brayns assets
ARG BRAYNS_SRC=/app/brayns
WORKDIR /app
ADD . ${BRAYNS_SRC}


# Install Brayns
# https://github.com/BlueBrain/Brayns

# TODO: "|| exit 0"  hack to be removed as soon as MVDTool export issue is fixed.
RUN cksum ${BRAYNS_SRC}/.gitsubprojects \
   && cd ${BRAYNS_SRC} \
   && git submodule update --init --recursive \
   && mkdir -p build \
   && cd build \
   && CMAKE_PREFIX_PATH=${DIST_PATH}:${DIST_PATH}/lib/cmake/libwebsockets \
   cmake ..  \
   -DBRAYNS_ASSIMP_ENABLED=ON \
   -DBRAYNS_OSPRAY_ENABLED=ON \
   -DBRAYNS_CIRCUITEXPLORER_ENABLED=ON \
   -DBRAYNS_CIRCUITINFO_ENABLED=ON \
   -DBRAYNS_DTI_ENABLED=OFF \
   -DCLONE_SUBPROJECTS=ON \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
   -DBUILD_PYTHON_BINDINGS=OFF \
   -DEXTLIB_FROM_SUBMODULES=ON

RUN cd ${BRAYNS_SRC}/build && make -j install \
   && rm -rf ${DIST_PATH}/include ${DIST_PATH}/cmake ${DIST_PATH}/share

# Final image, containing only Brayns and libraries required to run it
FROM debian:buster-slim
ARG DIST_PATH=/app/dist

RUN apt-get update \
   && apt-get -y --no-install-recommends install \
   libarchive13 \
   libassimp4 \
   libfreeimage3 \
   libgomp1 \
   libhdf5-103 \
   libturbojpeg0 \
   && apt-get clean \
   && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# The COPY command below will:
# 1. create a container based on the `builder` image (but do not start it)
#    Equivalent to the `docker create` command
# 2. create a new image layer containing the
#    /app/dist directory of this new container
#    Equivalent to the `docker copy` command.
ARG BOOST_LIB=/usr/local/lib

COPY --from=builder ${DIST_PATH} ${DIST_PATH}
COPY --from=builder ${BOOST_LIB} ${BOOST_LIB}

# Add binaries from dist to the PATH
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:${DIST_PATH}/lib:${BOOST_LIB}
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
CMD ["--uri", "0.0.0.0:8200", "--plugin", "braynsCircuitExplorer", "--plugin", "braynsCircuitInfo", "--sandbox-path", "/"]
