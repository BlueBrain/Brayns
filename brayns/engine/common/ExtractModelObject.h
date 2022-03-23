#pragma once

#include <brayns/common/TransferFunction.h>
#include <brayns/engine/Model.h>
#include <brayns/engine/Material.h>

namespace brayns
{
struct ExtractModelObject
{
    static Material &extractMaterial(Model &model);

    static TransferFunction &extractTransferFunction(Model &model);
};
}
