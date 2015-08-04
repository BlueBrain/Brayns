# Copyright (c) HBP 2014-2015 cyrille.favreau@epfl.ch
# All rights reserved. Do not distribute without further notice.

set(BRAYNS_HEADERS
  detail/attribute.h
)

set(BRAYNS_SOURCES
  detail/attribute.cpp
)

flatbuffers_generate_c_headers(FBS
  detail/attribute.fbs
)
