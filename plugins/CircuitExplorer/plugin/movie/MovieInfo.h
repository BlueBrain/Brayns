#pragma once

#include <string>
#include <vector>

struct MovieInfo
{
    std::string outputFile;
    std::vector<std::string> inputFiles;
    int width = 0;
    int height = 0;
    int framerate = 0;
    int64_t bitrate = 0;
    std::string codec;
    std::string format;
};