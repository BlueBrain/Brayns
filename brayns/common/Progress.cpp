/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 *
 * Responsible Author: Daniel.Nachbaur@epfl.ch
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "Progress.h"

#include <boost/progress.hpp>

#ifdef BRAYNS_USE_OPENMP
#include <omp.h>
#endif

namespace brayns
{
class Progress::Impl
{
public:
    Impl(const std::string& message, const size_t expectedCount,
         const UpdateCallback& updateCallback)
        : _display(expectedCount, std::cout,
                   "[INFO ] " + message + "\n[INFO ] ", "[INFO ] ", "[INFO ] ")
        , _message(message)
        , _updateCallback(updateCallback)
    {
        if (_updateCallback)
            _updateCallback(_message, 0);
    }

    ~Impl()
    {
        if (_updateCallback)
            _updateCallback(_message, 1);
    }

    boost::progress_display _display;
    std::string _message;
    Progress::UpdateCallback _updateCallback;
};

Progress::Progress(const std::string& message, const size_t expectedCount,
                   const UpdateCallback& updateCallback)
    : _impl(new Impl(message, expectedCount, updateCallback))

{
}

Progress::~Progress()
{
}

Progress::Progress(Progress&& rhs)
    : _impl(std::move(rhs._impl))
{
}

Progress& Progress::operator=(Progress&& rhs)
{
    if (this != &rhs)
        _impl = std::move(rhs._impl);
    return *this;
}

void Progress::setMessage(const std::string& message)
{
#ifdef BRAYNS_USE_OPENMP
    if (omp_get_thread_num() == 0)
#endif
        _impl->_message = message;

    operator+=(0);
}

void Progress::operator++()
{
    operator+=(1);
}

void Progress::operator+=(const size_t increment)
{
#pragma omp critical
    _impl->_display += increment;

    if (_impl->_updateCallback)
#ifdef BRAYNS_USE_OPENMP
        if (omp_get_thread_num() == 0)
#endif
            _impl->_updateCallback(_impl->_message,
                                   float(_impl->_display.count()) /
                                       _impl->_display.expected_count());
}

size_t Progress::getCurrent() const
{
    return _impl->_display.count();
}
}
