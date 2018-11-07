// ======================================================================== //
// Copyright 2009-2018 Intel Corporation                                    //
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

#include <brayns/common/types.h>

namespace brayns
{
class DynamicLib
{
public:
    DynamicLib(const std::string& name);
    ~DynamicLib();

    DynamicLib(DynamicLib&& other);
    DynamicLib& operator=(DynamicLib&& other);
    DynamicLib(const DynamicLib&) = delete;
    DynamicLib& operator=(const DynamicLib&) = delete;

    void* getSymbolAddress(const std::string& name) const;

private:
    void* _handler{nullptr};
};
}
