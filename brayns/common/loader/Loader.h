/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
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

#include <brayns/common/types.h>

#ifdef BRAYNS_USE_OPENMP
#include <omp.h>
#endif

namespace brayns
{
/**
 * A base class for data loaders to unify loading data from blobs and files, and
 * provide progress feedback.
 */
class Loader
{
public:
    virtual ~Loader() = default;

    /**
     * Import the data from the blob and add it to the scene.
     *
     * @param blob the blob containing the data to import
     * @param scene the scene where to add the loaded model to
     * @param transformation the transformation to apply for the added model
     * @param materialID the default material ot use
     */
    virtual void importFromBlob(Blob&& blob, Scene& scene,
                                const Matrix4f& transformation,
                                const size_t materialID) = 0;

    /**
     * Import the data from the given file and add it to the scene.
     *
     * @param filename the file containing the data to import
     * @param scene the scene where to add the loaded model to
     * @param transformation the transformation to apply for the added model
     * @param materialID the default material ot use
     */
    virtual void importFromFile(const std::string& filename, Scene& scene,
                                const Matrix4f& transformation,
                                const size_t materialID) = 0;

    /**
     * The callback for each progress update with the signature (message,
     * fraction of progress in 0..1 range)
     */
    using UpdateCallback = std::function<void(const std::string&, float)>;

    /** Set a new callback function which is called on each updateProgress(). */
    void setProgressCallback(const UpdateCallback& func)
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
    UpdateCallback _progressUpdate;
};
}
