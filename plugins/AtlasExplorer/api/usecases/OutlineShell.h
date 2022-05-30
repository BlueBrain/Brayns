#pragma once

#include <brayns/common/MathTypes.h>
#include <brayns/engine/Model.h>

struct OutlineShellData
{
    brayns::Vector3ui gridSize;
    brayns::Vector3f gridSpacing;
    std::vector<uint8_t> data;
};

class OutlineShell
{
public:
    static void generate(brayns::Model &model, OutlineShellData data);
};
