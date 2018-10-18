/* Copyright (c) 2018 EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Jonas Karlsson <jonas.karlsson@epfl.ch>
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

#include "ArchiveLoader.h"

#include <brayns/common/log.h>
#include <brayns/common/scene/Model.h>
#include <brayns/common/scene/Scene.h>
#include <brayns/common/utils/Utils.h>

#include <fstream>

#include <archive.h>
#include <archive_entry.h>

#include <boost/filesystem.hpp>
#include <boost/range.hpp>

namespace fs = boost::filesystem;

namespace
{
bool isSupportedArchiveType(const std::string& extension)
{
    // No way to get all supported types from libarchive...
    // Extend this list if you feel your favorite archive type should be here
    const std::set<std::string> extensions{"zip", "gz", "tgz", "bz2", "rar"};
    return extensions.find(extension) != extensions.end();
}

archive* _openArchive(const std::string& filename)
{
    auto archive = archive_read_new();
    archive_read_support_format_all(archive);
    archive_read_support_filter_all(archive);

    // non-tar archives like gz, bzip2, ... need to be added as raw
    auto extension = fs::extension(filename);
    if (!extension.empty())
    {
        if (isSupportedArchiveType(extension.erase(0, 1)))
            archive_read_support_format_raw(archive);
    }
    if (archive_read_open_filename(archive, filename.c_str(), 10240) ==
        ARCHIVE_OK)
    {
        return archive;
    }
    archive_read_free(archive);
    return nullptr;
}

archive* _openArchive(const brayns::Blob& blob)
{
    auto archive = archive_read_new();
    archive_read_support_format_all(archive);
    archive_read_support_filter_all(archive);

    // non-tar archives like gz, bzip2, ... need to be added as raw
    auto extension = fs::extension(blob.name);
    if (!extension.empty())
    {
        if (isSupportedArchiveType(extension.erase(0, 1)))
            archive_read_support_format_raw(archive);
    }
    if (archive_read_open_memory(archive, (void*)blob.data.data(),
                                 blob.data.size()) == ARCHIVE_OK)
    {
        return archive;
    }
    archive_read_free(archive);
    return nullptr;
}

int copy_data(struct archive* ar, struct archive* aw)
{
    for (;;)
    {
        const void* buff;
        size_t size;
        int64_t offset;

        auto r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return ARCHIVE_OK;
        if (r < ARCHIVE_OK)
            return r;
        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK)
        {
            std::cerr << archive_error_string(aw) << std::endl;
            return (r);
        }
    }
}

void _extractArchive(archive* archive, const std::string& filename,
                     const std::string& destination)
{
    auto writer = archive_write_disk_new();
    archive_write_disk_set_options(writer, 0);
    archive_write_disk_set_standard_lookup(writer);

    for (;;)
    {
        archive_entry* entry;
        auto r = archive_read_next_header(archive, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_OK)
            std::cerr << archive_error_string(archive) << std::endl;
        if (r < ARCHIVE_WARN)
        {
            throw std::runtime_error(
                std::string("Error reading file from archive: ") +
                archive_error_string(archive));
        }
        const char* currentFile = archive_entry_pathname(entry);

        // magic 'data' file for gzip archives is useless to us, so rename it
        if (std::string(currentFile) == "data")
            currentFile = filename.c_str();
        const std::string fullOutputPath = destination + "/" + currentFile;
        archive_entry_set_pathname(entry, fullOutputPath.c_str());
        r = archive_write_header(writer, entry);
        if (r < ARCHIVE_OK)
            std::cerr << archive_error_string(writer) << std::endl;
        else if (archive_entry_size(entry) > 0)
        {
            r = copy_data(archive, writer);
            if (r < ARCHIVE_OK)
                std::cerr << archive_error_string(writer) << std::endl;
            if (r < ARCHIVE_WARN)
                throw std::runtime_error(
                    std::string("Error writing file from archive to disk: ") +
                    archive_error_string(archive));
        }
        r = archive_write_finish_entry(writer);
        if (r < ARCHIVE_OK)
            std::cerr << archive_error_string(writer) << std::endl;
        if (r < ARCHIVE_WARN)
            throw std::runtime_error(
                std::string("Error finishing current file: ") +
                archive_error_string(archive));
    }
    archive_read_free(archive);
    archive_write_close(writer);
    archive_write_free(writer);
}

void extractFile(const std::string& filename, const std::string& destination)
{
    auto archive = _openArchive(filename);
    if (!archive)
        throw std::runtime_error(filename + " is not a supported archive type");
    _extractArchive(archive, fs::basename(filename), destination);
}

void extractBlob(brayns::Blob&& blob, const std::string& destination)
{
    auto archive = _openArchive(blob);
    if (!archive)
        throw std::runtime_error("Blob is not a supported archive type");
    _extractArchive(archive, fs::basename(blob.name), destination);
}
}

namespace brayns
{
ArchiveLoader::ArchiveLoader(Scene& scene, LoaderRegistry& registry)
    : Loader(scene)
    , _registry(registry)
{
}

bool ArchiveLoader::isSupported(const std::string& filename BRAYNS_UNUSED,
                                const std::string& extension) const
{
    return isSupportedArchiveType(extension);
}

ModelDescriptorPtr ArchiveLoader::loadExtracted(
    const std::string& path, const LoaderProgress& callback, const size_t index,
    const size_t defaultMaterialId) const
{
    for (const auto& i :
         boost::make_iterator_range(fs::directory_iterator(path), {}))
    {
        const std::string currPath = i.path().string();
        if (_registry.isSupportedFile(currPath))
        {
            const auto& loader = _registry.getLoaderFromFilename(currPath);
            return loader.importFromFile(currPath, callback, index,
                                         defaultMaterialId);
        }
    }
    throw std::runtime_error("No loader found for archive.");
}

ModelDescriptorPtr ArchiveLoader::importFromBlob(
    Blob&& blob, const LoaderProgress& callback,
    const size_t index BRAYNS_UNUSED,
    const size_t defaultMaterialId BRAYNS_UNUSED) const
{
    fs::path path = fs::temp_directory_path() / fs::unique_path();
    extractBlob(std::move(blob), path.string());
    return loadExtracted(path.string(), callback, index, defaultMaterialId);
}

ModelDescriptorPtr ArchiveLoader::importFromFile(
    const std::string& filename, const LoaderProgress& callback,
    const size_t index, const size_t defaultMaterialId) const
{
    fs::path path = fs::temp_directory_path() / fs::unique_path();
    extractFile(filename, path.string());
    return loadExtracted(path.string(), callback, index, defaultMaterialId);
}
}
