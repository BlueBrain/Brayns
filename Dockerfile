# Docker container for running Brayns as a service
# Check https://docs.docker.com/engine/userguide/eng-image/dockerfile_best-practices/#user for best practices.

# This Dockerfile leverages multi-stage builds, available since Docker 17.05
# See: https://docs.docker.com/engine/userguide/eng-image/multistage-build/#use-multi-stage-builds

# Image where Brayns is built
FROM debian:9.3-slim as builder
LABEL maintainer="bbp-svc-viz@groupes.epfl.ch"
ARG DIST_PATH=/app/dist

# Install packages
RUN apt-get update \
 && apt-get -y --no-install-recommends install \
    build-essential \
    cmake \
    git \
    ninja-build \
    libassimp-dev \
    libboost-date-time-dev \
    libboost-filesystem-dev \
    libboost-iostreams-dev \
    libboost-program-options-dev \
    libboost-regex-dev \
    libboost-serialization-dev \
    libboost-system-dev \
    libboost-test-dev \
    libhdf5-serial-dev \
    libmagick++-dev \
    libtbb-dev \
    libturbojpeg0-dev \
    qtbase5-dev \
    wget \
    ca-certificates \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Get ISPC
# https://ispc.github.io/downloads.html
ARG ISPC_VERSION=1.9.2
ARG ISPC_DIR=ispc-v${ISPC_VERSION}-linux
ARG ISPC_PATH=/app/$ISPC_DIR

RUN mkdir -p ${ISPC_PATH} \
 && wget http://netix.dl.sourceforge.net/project/ispcmirror/v${ISPC_VERSION}/${ISPC_DIR}.tar.gz \
 && tar zxvf ${ISPC_DIR}.tar.gz -C ${ISPC_PATH} --strip-components=1 \
 && rm -rf ${ISPC_PATH}/${ISPC_DIR}/examples

# Add ispc bin to the PATH
ENV PATH $PATH:${ISPC_PATH}

# Install embree
# https://github.com/embree/embree/releases
ARG EMBREE_VERSION=2.17.1
ARG EMBREE_FILE=embree-${EMBREE_VERSION}.x86_64.linux.tar.gz

RUN mkdir -p ${DIST_PATH} \
  && wget https://github.com/embree/embree/releases/download/v${EMBREE_VERSION}/${EMBREE_FILE} \
  && tar zxvf ${EMBREE_FILE} -C ${DIST_PATH} --strip-components=1 \
  && rm -rf ${DIST_PATH}/bin ${DIST_PATH}/doc

# Install OSPRay
# https://github.com/ospray/ospray/releases
ARG OSPRAY_VERSION=1.4.3
ARG OSPRAY_SRC=/app/ospray

RUN mkdir -p ${OSPRAY_SRC} \
 && git clone https://github.com/ospray/ospray.git ${OSPRAY_SRC} --depth 1 \
 && cd ${OSPRAY_SRC} \
 && git checkout v${OSPRAY_VERSION} \
 && mkdir -p build \
 && cd build \
 && CMAKE_PREFIX_PATH=${DIST_PATH} cmake .. -GNinja \
    -DOSPRAY_ENABLE_APPS=OFF \
    -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
 && ninja install

# Install libwebsockets (2.0 from Debian is not reliable)
# https://github.com/warmcat/libwebsockets/releases
ARG LWS_VERSION=1.7.1
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
    -DLWS_WITHOUT_EXTENSIONS=ON \
    -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
 && ninja install \
 && rm -rf ${DIST_PATH}/lib/cmake/libwebsockets


# Set working dir and copy Brayns assets
ARG BRAYNS_SRC=/app/brayns
WORKDIR /app
ADD . ${BRAYNS_SRC}


# Install Brayns
# https://github.com/BlueBrain/Brayns
RUN cksum ${BRAYNS_SRC}/.gitsubprojects \
 && cd ${BRAYNS_SRC} \
 && git submodule update --init --recursive --remote \
 && mkdir -p build \
 && cd build \
 && PKG_CONFIG_PATH=${DIST_PATH}/lib/pkgconfig CMAKE_PREFIX_PATH=${DIST_PATH} cmake .. -GNinja \
    -DBRAYNS_BRION_ENABLED=ON \
    -DBRAYNS_DEFLECT_ENABLED=ON \
    -DBRAYNS_NETWORKING_ENABLED=ON \
    -DCLONE_SUBPROJECTS=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
    -DBUILD_PYTHON_BINDINGS=OFF \
 && ninja mvd-tool Brayns-install \
 && rm -rf ${DIST_PATH}/include ${DIST_PATH}/share

# Final image, containing only Brayns and libraries required to run it
FROM debian:9.3-slim
ARG DIST_PATH=/app/dist

RUN apt-get update \
 && apt-get -y --no-install-recommends install \
    libassimp3v5 \
    libboost-filesystem1.62.0 \
    libboost-program-options1.62.0 \
    libboost-regex1.62.0 \
    libboost-serialization1.62.0 \
    libboost-system1.62.0 \
    libboost-iostreams1.62.0 \
    libgomp1 \
    libhdf5-100 \
    libhdf5-cpp-100 \
    libmagick++-6.q16-7 \
    libmagickwand-6.q16-3 \
    libqt5concurrent5 \
    libqt5network5 \
    libturbojpeg0 \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

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
