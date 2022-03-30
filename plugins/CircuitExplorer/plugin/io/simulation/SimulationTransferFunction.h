#pragma once

#include <brayns/common/TransferFunction.h>
#include <brayns/engine/common/DataHandler.h>

struct SimulationTransferFunction
{
    static void setUnipolarColormap(brayns::TransferFunction &tf) noexcept;
    static brayns::OSPBuffer sampleAsBuffer(const brayns::TransferFunction &tf) noexcept;
};
