#pragma once

#include <brayns/engine/ModelComponents.h>
#include <brayns/engine/geometries/Primitive.h>

#include <plugin/io/morphology/neuron/NeuronSection.h>

#include <ospray/ospray.h>

struct MorphologySectionMapping
{
    NeuronSection section;
    size_t begin;
    size_t end;
};

class MorphologyCircuitComponent final : public brayns::Component
{
public:
    size_t getSizeInBytes() const noexcept override;

    void onStart() override;

    bool commit() override;

    void onDestroyed() override;

    void addMorphology(uint64_t id, MorphologySectionMapping morphology) noexcept;

    const std::vector<uint64_t> &getIDs() const noexcept;

    const std::vector<MorphologySectionMapping> &getMapping() const noexcept;

private:
    OSPGeometricModel _model = nullptr;
    brayns::Geometry<brayns::Primitive> _geometry;

    std::vector<uint64_t> _ids;
    std::vector<MorphologySectionMapping> _morphologies;

    std::vector<brayns::Vector4f> _colors;
    std::vector<uint8_t> _colorIndices;
};
