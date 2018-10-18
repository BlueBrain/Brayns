// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#undef NDEBUG

#include <ospray/SDK/common/Data.h>

#include "Cones.h"

namespace bbp
{
namespace optix
{
std::string Cones::toString() const
{
    return "optix::Cones";
}

void Cones::finalize(Model* optixModel)
{
    ospray::Ref<ospray::Data> data = getParamData("cones", nullptr);
    if (!data.ptr)
        throw std::runtime_error("#optix:geometry/cones: "
                                 "no 'cones' data specified");

    Geometry::finalize(optixModel);

    const size_t bytesPerCone =
        getParam1i("bytes_per_cone", 11 * sizeof(float));
    _context["cone_size"]->setUint(bytesPerCone);
    _geometry->setPrimitiveCount(data->numBytes / bytesPerCone);

    _setBuffer("cones", data);
}
}
}
