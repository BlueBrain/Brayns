# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

apt-get update && apt-get -y --no-install-recommends install \
   build-essential \
   cmake \
   git \
   ninja-build \
   libhdf5-dev \
   wget \
   ca-certificates \
   libssl-dev \
   zlib1g-dev \
   libbz2-dev \
   && apt-get clean \
   && rm -rf /var/lib/apt/lists
