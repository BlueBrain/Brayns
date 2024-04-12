# Docker container for running Brayns as a service
# Check https://docs.docker.com/engine/userguide/eng-image/dockerfile_best-practices/#user for best practices.

# This Dockerfile leverages multi-stage builds, available since Docker 17.05
# See: https://docs.docker.com/engine/userguide/eng-image/multistage-build/#use-multi-stage-builds

# Image where Brayns is built
FROM ubuntu:22.04 as builder

LABEL maintainer="bbp-svc-viz@groupes.epfl.ch"

ARG DIST_PATH=/app/dist
ARG BRAYNS_SRC=/app/Brayns

# Install packages
RUN apt-get update && apt-get -y --no-install-recommends install \
   build-essential \
   cmake \
   git \
   ninja-build \
   libhdf5-serial-dev \
   wget \
   ca-certificates \
   libssl-dev \
   zlib1g-dev \
   libbz2-dev \
   && apt-get clean

# Copy Brayns assets
ADD apps ${BRAYNS_SRC}/apps
ADD cmake ${BRAYNS_SRC}/cmake
ADD scripts/superbuild/CMakeLists.txt ${BRAYNS_SRC}/scripts/superbuild/CMakeLists.txt
ADD src ${BRAYNS_SRC}/src
ADD CMakeLists.txt ${BRAYNS_SRC}/CMakeLists.txt

# Configure Brayns superbuild
RUN cd ${BRAYNS_SRC}/scripts/superbuild \
   && mkdir -p build && cd build \
   && cmake ..  \
   -GNinja \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
   -DCMAKE_BUILD_TYPE=Release \
   -DBRAYNS_ENABLE_INSTALL=ON

# Build Brayns and dependencies
RUN cd ${BRAYNS_SRC}/scripts/superbuild/build \
   && cmake --build . -j4 \
   && rm -rf ${DIST_PATH}/include ${DIST_PATH}/lib/cmake ${DIST_PATH}/share

# Final image, containing only Brayns and libraries required to run it
FROM ubuntu:22.04

ARG DIST_PATH=/app/dist

RUN apt-get update && apt-get -y --no-install-recommends install \
   libgomp1 \
   libhdf5-103 \
   libssl-dev \
   && apt-get clean

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
EXPOSE 5000

# When running `docker run -ti --rm -p 5000:5000 brayns`,
# this will be the cmd that will be executed (+ the CLI options from CMD).
# To ssh into the container (or override the default entry) use:
# `docker run -ti --rm --entrypoint bash -p 5000:5000 brayns`
# See https://docs.docker.com/engine/reference/run/#entrypoint-default-command-to-execute-at-runtime
# for more docs
ENTRYPOINT ["braynsService"]
CMD ["--uri", "0.0.0.0:5000"]
