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

namespace brayns
{
/**
 * A base class for any loader who wishes to report progress during loading
 * operations.
 */
class ProgressReporter
{
public:
    ProgressReporter() = default;
    ~ProgressReporter() = default;

    /** Set a new callback function which is called on each updateProgress(). */
    void setProgressUpdate(const Progress::ProgressUpdateCallback& func)
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
        if (_progressUpdate)
            _progressUpdate(message, current, expected);
    }

private:
    Progress::ProgressUpdateCallback _progressUpdate;
};
}
