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

# Install OSPRay
ARG OSPRAY_TAG=v3.1.0
ARG OSPRAY_SRC=/app/ospray

RUN mkdir -p ${OSPRAY_SRC} \
   && git clone https://github.com/ospray/ospray.git ${OSPRAY_SRC} \
   && cd ${OSPRAY_SRC} \
   && git checkout ${OSPRAY_TAG} \
   && mkdir -p build \
   && cd build \
   && cmake ../scripts/superbuild \
   -DCMAKE_BUILD_TYPE=Release \
   -DINSTALL_IN_SEPARATE_DIRECTORIES=OFF \
   -DBUILD_GLFW=OFF \
   -DBUILD_OIDN=OFF \
   -DBUILD_OSPRAY_APPS=OFF \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
   && cmake --build .

# Set working dir and copy Brayns assets
ARG BRAYNS_SRC=/app/brayns
WORKDIR /app
ADD . ${BRAYNS_SRC}

# Install Brayns
# https://github.com/BlueBrain/Brayns
RUN cd ${BRAYNS_SRC} \
   && mkdir -p build \
   && cd build \
   && cmake ..  \
   -GNinja \
   -DCMAKE_PREFIX_PATH=${DIST_PATH} \
   -DBRAYNS_ENABLE_TESTS=OFF \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH}

RUN cd ${BRAYNS_SRC}/build \
   && ninja -j4 install \
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
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:${DIST_PATH}/lib:/${DIST_PATH}/lib/intel64/gcc4.8
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
