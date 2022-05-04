/* Copyright (c) 2015-2022, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nadir Roman Guerrero <nadir.romanguerrero@epfl.ch>
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

#include "NRRDLoader.h"

std::vector<std::string> NRRDLoader::getSupportedExtensions() const
{
    return {".nrrd"};
}

std::string NRRDLoader::getName() const
{
    return "NRRD Loader";
}

std::vector<std::unique_ptr<brayns::Model>> NRRDLoader::importFromBlob(
    brayns::Blob &&blob,
    const brayns::LoaderProgress &callback) const
{
    (void)blob;
    (void)callback;
    throw std::runtime_error("NRRD Loader does not support blob loading");
}

std::vector<std::unique_ptr<brayns::Model>> NRRDLoader::importFromFile(
    const std::string &path,
    const brayns::LoaderProgress &callback) const
{
}
