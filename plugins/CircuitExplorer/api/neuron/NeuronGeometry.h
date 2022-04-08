#pragma once

#include <brayns/engine/geometries/Primitive.h>

#include "NeuronSection.h"

#include <unordered_map>
#include <vector>

struct NeuronSectionMapping
{
    NeuronSection type;
    size_t begin;
    size_t end;
};

struct NeuronGeometry
{
    std::vector<brayns::Primitive> geometry;
    std::vector<NeuronSectionMapping> sectionMapping;
    std::unordered_map<int32_t, std::vector<size_t>> sectionSegmentMapping;
};
