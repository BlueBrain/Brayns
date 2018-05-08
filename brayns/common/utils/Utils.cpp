/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
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

#include "Utils.h"

#include <brayns/common/log.h>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#ifdef BRAYNS_USE_LIBARCHIVE
#include <archive.h>
#include <archive_entry.h>
#endif

namespace brayns
{
strings parseFolder(const std::string& folder, const strings& filters)
{
    strings files;
    fs::directory_iterator endIter;
    if (fs::is_directory(folder))
    {
        for (fs::directory_iterator dirIter(folder); dirIter != endIter;
             ++dirIter)
        {
            if (fs::is_regular_file(dirIter->status()))
            {
                const auto filename = dirIter->path().c_str();
                if (filters.empty())
                    files.push_back(filename);
                else
                {
                    const auto& fileExtension = dirIter->path().extension();
                    const auto found = std::find(filters.begin(), filters.end(),
                                                 fileExtension);
                    if (found != filters.end())
                        files.push_back(filename);
                }
            }
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

const std::string ELLIPSIS("...");

std::string shortenString(const std::string& string, const size_t maxLength)
{
    if (string.length() <= maxLength)
        return string;

    const float spacePerPart = (maxLength - ELLIPSIS.length()) / 2.f;
    const auto beforeEllipsis = string.substr(0, std::ceil(spacePerPart));
    const auto afterEllipsis =
        string.substr(string.size() - std::floor(spacePerPart));

    return beforeEllipsis + ELLIPSIS + afterEllipsis;
}

bool isSupportedArchiveType(const std::string& extension BRAYNS_UNUSED)
{
#ifdef BRAYNS_USE_LIBARCHIVE
    // No way to get all supported types from libarchive...
    // Extend this list if you feel your favorite archive type should be here
    const std::set<std::string> extensions{"zip", "gz", "tgz", "bz2", "rar"};
    return extensions.find(extension) != extensions.end();
#else
    return false;
#endif
}

#ifdef BRAYNS_USE_LIBARCHIVE
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
        return archive;
    return nullptr;
}

archive* _openArchive(const Blob& blob)
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
#endif

bool isArchive(const std::string& filename BRAYNS_UNUSED)
{
#ifdef BRAYNS_USE_LIBARCHIVE
    auto archive = _openArchive(filename);
    if (!archive)
        return false;
    archive_read_free(archive);
    return true;
#else
    return false;
#endif
}

bool isArchive(const Blob& blob BRAYNS_UNUSED)
{
#ifdef BRAYNS_USE_LIBARCHIVE
    auto archive = _openArchive(blob);
    if (!archive)
        return false;
    archive_read_free(archive);
    return true;
#else
    return false;
#endif
}

void extractFile(const std::string& filename BRAYNS_UNUSED,
                 const std::string& destination BRAYNS_UNUSED)
{
#ifdef BRAYNS_USE_LIBARCHIVE
    auto archive = _openArchive(filename);
    if (!archive)
        throw std::runtime_error(filename + " is not a supported archive type");
    _extractArchive(archive, fs::basename(filename), destination);
#else
    throw std::runtime_error("No support for archives; missing libarchive");
#endif
}

void extractBlob(Blob&& blob BRAYNS_UNUSED,
                 const std::string& destination BRAYNS_UNUSED)
{
#ifdef BRAYNS_USE_LIBARCHIVE
    auto archive = _openArchive(blob);
    if (!archive)
        throw std::runtime_error("Blob is not a supported archive type");
    _extractArchive(archive, fs::basename(blob.name), destination);
#else
    throw std::runtime_error("No support for archives; missing libarchive");
#endif
}
}
