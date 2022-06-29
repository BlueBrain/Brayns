#pragma once

#include "StreamlineData.h"

#include <map>

namespace dti
{
class RowStreamlineMapReader
{
public:
    template<typename RowFilterCallback>
    static std::map<uint64_t, StreamlineData> read(const std::string &path, const RowFilterCallback &filter)
    {
        std::map<uint64_t, StreamlineData> result;

        const auto content = brayns::FileReader::read(path);
        std::istringstream stream(content);

        size_t row = 0;
        size_t index = 0;

        while (stream.good())
        {
            std::string line;
            std::getline(stream, line);

            if (!line.empty() && filter(row))
            {
                std::istringstream lineStream(line);
                uint64_t nbPoints;
                lineStream >> nbPoints;

                if (!lineStream.good() || nbPoints == 0)
                {
                    throw std::runtime_error("Row " + std::to_string(row));
                }

                auto &streamline = result[row];

                streamline.linealIndex = index++;
                streamline.points = _readPoints(lineStream, nbPoints);
            }

            ++row;
        }

        return result;
    }

private:
    static std::vector<brayns::Vector3f> _readPoints(std::istringstream &lineStream, size_t nbPoints)
    {
        std::vector<brayns::Vector3f> points;
        points.reserve(nbPoints);

        for (uint64_t i = 0; i < nbPoints && lineStream.good(); ++i)
        {
            brayns::Vector3f point;
            lineStream >> point.x >> point.y >> point.z;
            points.push_back(point);
        }

        return points;
    }
};
}
