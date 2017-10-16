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

#pragma once

#include <brayns/common/Progress.h>

#include <functional>

#ifdef BRAYNS_USE_OPENMP
#include <omp.h>
#endif

namespace brayns
{
/**
 * A base class for any loader who wishes to report progress during loading
 * operations.
 */
class ProgressReporter
{
public:
    virtual ~ProgressReporter() = default;

    /** Set a new callback function which is called on each updateProgress(). */
    void setProgressCallback(const Progress::UpdateCallback& func)
    {
        _progressUpdate = func;
    }

    /**
     * Update the current progress of an operation. Will call the provided
     * callback from setProgressUpdate().
     */
    void updateProgress(const std::string& message, const size_t current,
                        const size_t expected)
    {
#ifdef BRAYNS_USE_OPENMP
        if (omp_get_thread_num() == 0)
#endif
            if (_progressUpdate)
                _progressUpdate(message, float(current) / expected);
    }

private:
    Progress::UpdateCallback _progressUpdate;
};
}
