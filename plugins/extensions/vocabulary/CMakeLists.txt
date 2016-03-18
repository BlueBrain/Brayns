# Copyright (c) HBP 2014-2016 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.

set(BRAYNSZEROBUFRENDER_INCLUDE_NAME zerobuf/render)

set(BRAYNSZEROBUFRENDER_PUBLIC_HEADERS ${ZEROBUFRENDER_HEADERS})
set(BRAYNSZEROBUFRENDER_LINK_LIBRARIES PUBLIC zeq zeqHBP ZeroBuf ZeroBufRender)

set(BRAYNSZEROBUFRENDER_INCLUDE_NAME zerobuf/render)
set(BRAYNSZEROBUFRENDER_NAMESPACE zerobufrender)

include(zerobufGenerateCxx)
zerobuf_generate_cxx(
  BRAYNSZEROBUFRENDER
  ${OUTPUT_INCLUDE_DIR}/${BRAYNSZEROBUFRENDER_INCLUDE_NAME}
  attribute.fbs
  frameBuffers.fbs
)

common_library(BraynsZeroBufRender)
