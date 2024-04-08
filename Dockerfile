# Docker container for running Brayns as a service
# Check https://docs.docker.com/engine/userguide/eng-image/dockerfile_best-practices/#user for best practices.

# This Dockerfile leverages multi-stage builds, available since Docker 17.05
# See: https://docs.docker.com/engine/userguide/eng-image/multistage-build/#use-multi-stage-builds

# Image where Brayns is built
FROM ubuntu:22.04 as builder
LABEL maintainer="bbp-svc-viz@groupes.epfl.ch"
ARG DIST_PATH=/app/dist

# Install packages
RUN apt-get update \
   && apt-get -y --no-install-recommends install \
   build-essential \
   cmake \
   git \
   ninja-build \
   libhdf5-serial-dev \
   pkg-config \
   wget \
   ca-certificates \
   libssl-dev \
   zlib1g-dev \
   libbz2-dev \
   python3.9 \
   && apt-get clean \
   && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Get ISPC
ARG ISPC_VERSION=1.18.0
ARG ISPC_DIR=ispc-v${ISPC_VERSION}-linux
ARG ISPC_PATH=/app/ispc-v1.18.0-linux
RUN mkdir -p ${ISPC_PATH} \
   && wget https://github.com/ispc/ispc/releases/download/v1.18.0/${ISPC_DIR}.tar.gz \
   && tar zxvf ${ISPC_DIR}.tar.gz -C ${ISPC_PATH} --strip-components=1 \
   && rm -rf ${ISPC_PATH}/${ISPC_DIR}/examples

# Add ispc bin to the PATH
ENV PATH $PATH:${ISPC_PATH}

RUN mkdir -p ${DIST_PATH}

# Install One TBB
ARG ONETBB_VERSION=2021.5.0
ARG ONETBB_FILE=oneapi-tbb-${ONETBB_VERSION}-lin.tgz
RUN wget https://github.com/oneapi-src/oneTBB/releases/download/v${ONETBB_VERSION}/${ONETBB_FILE} \
   && tar zxvf ${ONETBB_FILE} -C ${DIST_PATH} --strip-components=1

# Install embree
ARG EMBREE_VERSION=3.13.3
ARG EMBREE_FILE=embree-${EMBREE_VERSION}.x86_64.linux.tar.gz
RUN wget https://github.com/embree/embree/releases/download/v${EMBREE_VERSION}/${EMBREE_FILE} \
   && tar zxvf ${EMBREE_FILE} -C ${DIST_PATH} --strip-components=1 \
   && rm -rf ${DIST_PATH}/bin ${DIST_PATH}/doc

# Install rk common
ARG RKCOMMON_VERSION=v1.10.0
ARG RKCOMMON_SRC=/app/rkcommon
RUN mkdir ${RKCOMMON_SRC} \
   && git clone https://github.com/ospray/rkcommon ${RKCOMMON_SRC} \
   && cd ${RKCOMMON_SRC} \
   && git checkout ${RKCOMMON_VERSION} \
   && mkdir build \
   && cd build \
   && cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=${DIST_PATH} -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
   && ninja -j4 install

# Install open vkl
ARG OPENVKL_VERSION=v1.3.0
ARG OPENVKL_SRC=/app/openvkl
RUN mkdir ${OPENVKL_SRC} \
   && git clone https://github.com/openvkl/openvkl ${OPENVKL_SRC} \
   && cd ${OPENVKL_SRC} \
   && git checkout ${OPENVKL_VERSION} \
   && mkdir build \
   && cd build \
   && cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_PREFIX_PATH=${DIST_PATH} \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
   -DBUILD_EXAMPLES=OFF \
   -DISPC_EXECUTABLE=${ISPC_PATH}/bin/ispc \
   && ninja -j4 install

# Install OSPRay
ARG OSPRAY_TAG=v2.10.5
ARG OSPRAY_SRC=/app/ospray

RUN mkdir -p ${OSPRAY_SRC} \
   && git clone https://github.com/BlueBrain/ospray.git ${OSPRAY_SRC} \
   && cd ${OSPRAY_SRC} \
   && git checkout ${OSPRAY_TAG} \
   && mkdir -p build \
   && cd build \
   && CMAKE_PREFIX_PATH=${DIST_PATH} cmake .. -GNinja \
   -DOSPRAY_ENABLE_APPS_TUTORIALS=OFF \
   -DOSPRAY_ENABLE_APPS_BENCHMARK=OFF \
   -DOSPRAY_ENABLE_APPS_EXAMPLES=OFF \
   -DOSPRAY_ENABLE_APPS_TESTING=OFF \
   -DOSPRAY_APPS_ENABLE_GLM=OFF \
   -DOSPRAY_MODULE_DENOISER=OFF \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
   -DISPC_EXECUTABLE=${ISPC_PATH}/bin/ispc \
   && ninja -j4 install

# Set working dir and copy Brayns assets
ARG BRAYNS_SRC=/app/brayns
WORKDIR /app
ADD . ${BRAYNS_SRC}

# Install Brayns
# https://github.com/BlueBrain/Brayns
RUN cd ${BRAYNS_SRC} \
   && mkdir -p build \
   && cd build \
   && CMAKE_PREFIX_PATH=${DIST_PATH} \
   cmake ..  \
   -DBRAYNS_ENABLE_TESTS=OFF \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH}

RUN cd ${BRAYNS_SRC}/build && make -j4 install \
   && rm -rf ${DIST_PATH}/include ${DIST_PATH}/cmake ${DIST_PATH}/share

# Final image, containing only Brayns and libraries required to run it
FROM ubuntu:22.04
ARG DIST_PATH=/app/dist

RUN apt-get update \
   && apt-get -y --no-install-recommends install \
   libgomp1 \
   libhdf5-103 \
   libssl-dev \
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
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:${DIST_PATH}/lib:${BOOST_LIB}:/${DIST_PATH}/lib/intel64/gcc4.8
ENV PATH ${DIST_PATH}/bin:$PATH

# Expose a port from the container
# For more ports, use the `--expose` flag when running the container,
# see https://docs.docker.com/engine/reference/run/#expose-incoming-ports for docs.
EXPOSE 5000

# When running `docker run -ti --rm -p 5000:5000 brayns`,
# this will be the cmd that will be executed (+ the CLI options from CMD).
# To ssh into the container (or override the default entry) use:
# `docker run -ti --rm --entrypoint bash -p 5000:5000 brayns`
# See https://docs.docker.com/engine/reference/run/#entrypoint-default-command-to-execute-at-runtime
# for more docs
ENTRYPOINT ["braynsService"]
CMD ["--uri", "0.0.0.0:5000"]
