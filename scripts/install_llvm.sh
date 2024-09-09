# Copyright (c) 2015-2024, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Adrien Fleury <adrien.fleury@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
./llvm.sh 20

apt-get install clang-format-20
apt-get install clang-tidy-20
