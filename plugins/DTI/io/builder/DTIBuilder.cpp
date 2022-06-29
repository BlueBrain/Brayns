#include "DTIBuilder.h"

#include "common/RowTreamlineMapReader.h"

#include <map>

namespace dti
{
void DTIBuilder::reset()
{
}

void DTIBuilder::readGidRowFile(const std::string &path)
{
    (void)path;
}

void DTIBuilder::readStreamlinesFile(const std::string &path)
{
    _streamlines = RowStreamlineMapReader::read(
        path,
        [](size_t row)
        {
            (void)row;
            return true;
        });
}

void DTIBuilder::buildGeometry(float radius, brayns::Model &model)
{
}

void DTIBuilder::buildSimulation(const std::string &path, float spikeDecayTime, brayns::Model &model)
{
}
}
