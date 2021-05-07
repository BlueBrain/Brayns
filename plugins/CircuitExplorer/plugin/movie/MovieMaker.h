#pragma once

#include "MovieInfo.h"
#include "MovieMakerException.h"

class MovieMaker
{
public:
    static void createMovie(const MovieInfo& info);
};