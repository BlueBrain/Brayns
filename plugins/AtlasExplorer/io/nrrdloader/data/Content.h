#pragma once

#include <cstdint>
#include <vector>

class IDataContent
{
public:
    virtual ~IDataContent() = default;

    virtual std::vector<char> asChars() const = 0;
    virtual std::vector<uint8_t> asBytes() const = 0;
    virtual std::vector<int16_t> asShorts() const = 0;
    virtual std::vector<uint16_t> asUnsingedShorts() const = 0;
    virtual std::vector<int32_t> asIntegers() const = 0;
    virtual std::vector<uint32_t> asUnsignedIntegers() const = 0;
    virtual std::vector<int64_t> asLongs() const = 0;
    virtual std::vector<uint64_t> asUnsignedLongs() const = 0;
    virtual std::vector<float> asFloats() const = 0;
    virtual std::vector<double> asDoubles() const = 0;
};
