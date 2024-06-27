# Image where Brayns is built
FROM ubuntu:22.04 as builder

LABEL maintainer="bbp-svc-viz@groupes.epfl.ch"

ARG DIST_PATH=/app/dist
ARG BRAYNS_SRC=/app/Brayns

# Copy Brayns assets
COPY apps ${BRAYNS_SRC}/apps
COPY cmake ${BRAYNS_SRC}/cmake
COPY deps ${BRAYNS_SRC}/deps
COPY scripts/superbuild/CMakeLists.txt ${BRAYNS_SRC}/scripts/superbuild/CMakeLists.txt
COPY scripts/install_packages.sh ${BRAYNS_SRC}/scripts/install_packages.sh
COPY src ${BRAYNS_SRC}/src
COPY CMakeLists.txt ${BRAYNS_SRC}/CMakeLists.txt
COPY CMakePresets.json ${BRAYNS_SRC}/CMakePresets.json

# Install APT packages
RUN bash ${BRAYNS_SRC}/scripts/install_packages.sh

# Configure Brayns superbuild
RUN cd ${BRAYNS_SRC}/scripts/superbuild \
   && mkdir -p build && cd build \
   && cmake ..  \
   -GNinja \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH} \
   -DCMAKE_BUILD_TYPE=Release

# Build Brayns dependencies
RUN cd ${BRAYNS_SRC}/scripts/superbuild/build \
   && cmake --build . -j4

# Configure Brayns with distribution preset
RUN cd ${BRAYNS_SRC} \
   && mkdir -p build && cd build \
   && cmake .. \
   --preset distribution \
   -DCMAKE_PREFIX_PATH=${DIST_PATH} \
   -DCMAKE_INSTALL_PREFIX=${DIST_PATH}

# Build Brayns with distribution preset
RUN cd ${BRAYNS_SRC} \
   && cmake --build --preset distribution -j4 \
   && cmake --install build/distribution

# Only keep runtime libraries
RUN rm -rf \
   ${DIST_PATH}/lib/cmake \
   ${DIST_PATH}/lib/pkgconfig \
   ${DIST_PATH}/lib/libsonata.a

# Final image, containing only Brayns and libraries required to run it
FROM ubuntu:22.04

ARG DIST_PATH=/app/dist

# Install only runtime dependencies
RUN apt-get update && apt-get -y --no-install-recommends install \
   libgomp1 \
   libhdf5-103 \
   libssl-dev \
   && apt-get clean \
   && rm -rf /var/lib/apt/lists

# Copy only runtime executables and libraries
COPY --from=builder ${DIST_PATH}/bin/braynsService ${DIST_PATH}/bin/braynsService
COPY --from=builder ${DIST_PATH}/lib ${DIST_PATH}/lib

# Add binaries from dist to the PATH
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:${DIST_PATH}/lib
ENV PATH ${DIST_PATH}/bin:$PATH

# Default entrypoint and exposed port
EXPOSE 5000
ENTRYPOINT ["braynsService"]
