# Copyright (c) 2017, EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
# Responsible Author: Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#
# This file is part of https://github.com/BlueBrain/ospray-modules

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(ospray_module_deflect VERSION 1.0.0)
set(ospray_module_deflect_VERSION_ABI 1)

include(Common)

common_find_package(Deflect REQUIRED)
common_find_package(OpenMP)
common_find_package(ospray REQUIRED SYSTEM)
common_find_package_post()

set(OSPRAY_MODULE_DEFLECT_OMIT_LIBRARY_HEADER ON)
set(OSPRAY_MODULE_DEFLECT_OMIT_VERSION_HEADERS ON)
set(OSPRAY_MODULE_DEFLECT_HEADERS DeflectPixelOp.h)
set(OSPRAY_MODULE_DEFLECT_SOURCES DeflectPixelOp.cpp)
set(OSPRAY_MODULE_DEFLECT_LINK_LIBRARIES PRIVATE Deflect)
common_library(ospray_module_deflect)
