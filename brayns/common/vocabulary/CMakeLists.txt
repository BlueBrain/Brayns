# Copyright (c) 2015-2016, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>

set(BRAYNSZEROBUFRENDER_INCLUDE_NAME zerobuf/render)

set(BRAYNSZEROBUFRENDER_PUBLIC_HEADERS ${ZEROBUFRENDER_HEADERS})
set(BRAYNSZEROBUFRENDER_LINK_LIBRARIES PUBLIC ZeroEQ ZeroBuf)

set(BRAYNSZEROBUFRENDER_INCLUDE_NAME zerobuf/render)
set(BRAYNSZEROBUFRENDER_NAMESPACE zerobufrender)

include(zerobufGenerateCxx)
zerobuf_generate_cxx(
  BRAYNSZEROBUFRENDER
  ${PROJECT_BINARY_DIR}/include/${BRAYNSZEROBUFRENDER_INCLUDE_NAME}
  attribute.fbs
  camera.fbs
  frameBuffers.fbs
  parameters.fbs
  reset.fbs
  scene.fbs
  spikes.fbs
  transferFunction1D.fbs
)

common_library(BraynsZeroBufRender)
