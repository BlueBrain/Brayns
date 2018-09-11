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

#pragma once

#include "Geometry.h"

namespace bbp
{
namespace optix
{
struct TriangleMesh : public Geometry
{
    TriangleMesh()
        : Geometry(Type::TriangleMesh)
    {
    }
    ~TriangleMesh() override;
    std::string toString() const override;
    void finalize(Model* model) override;

private:
    ::optix::Buffer _verticesBuffer;
    ::optix::Buffer _indicesBuffer;
    ::optix::Buffer _normalsBuffer;
    ::optix::Buffer _textureCoordsBuffer;
};
}
}
