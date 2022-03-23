#include "ExtractModelObject.h"

#include <brayns/engine/components/MaterialComponent.h>
#include <brayns/engine/components/TransferFunctionComponent.h>

namespace brayns
{
Material &ExtractModelObject::extractMaterial(Model &model)
{
    auto &component = model.getComponent<MaterialComponent>();
    return component.getMaterial();
}

TransferFunction &ExtractModelObject::extractTransferFunction(Model &model)
{
    auto &component = model.getComponent<TransferFunctionComponent>();
    return component.getTransferFunction();
}
}
