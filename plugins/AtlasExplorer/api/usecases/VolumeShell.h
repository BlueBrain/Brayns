#pragma once

#include <brayns/engine/Model.h>
#include <brayns/engine/volumes/RegularVolume.h>

#include <api/NRRDImage.h>

class VolumeShell
{
public:
    static brayns::RegularVolume generateShellVolume(const NRRDImage &image, brayns::Model &model)
    {
        const auto &header = image.getHeader();
        const auto &data = image.getData();

        auto dblData = data.asDoubles();
    }

private:
    static bool _validVoxel(const std::vector<double> &data, size_t start, size_t length)
    {
        size_t zeroCount = 0;
        for (size_t i = start; i < start + length; ++i)
        {
            if (!std::isfinite(data[i]))
            {
                return false;
            }

            if (data[i] == 0.0)
            {
                ++zeroCount;
            }
        }

        return zeroCount < length;
    }
};
