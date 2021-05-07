#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

class FileSystemHelper
{
public:
    using DirectoryIterator = boost::filesystem::directory_iterator;
    using Path = boost::filesystem::path;

    static bool isDirectory(const std::string& path)
    {
        return boost::filesystem::is_directory(path);
    }

    static void removeFiles(const std::vector<std::string>& entries)
    {
        for (const auto& entry : entries)
        {
            boost::filesystem::remove(entry);
        }
    }

    static void removeDirectory(const std::string& path)
    {
        boost::filesystem::remove_all(path);
    }

    static bool hasExtension(const std::string& path,
                             const std::string& extension)
    {
        return boost::algorithm::ends_with(path, extension);
    }

    static std::vector<std::string> getDirectoryFiles(
        const std::string& path, const std::string& extension)
    {
        std::vector<std::string> entries;
        for (const auto& entry : DirectoryIterator(path))
        {
            if (!boost::filesystem::is_regular_file(entry))
            {
                continue;
            }
            auto& native = entry.path().native();
            if (!hasExtension(native, extension))
            {
                continue;
            }
            entries.push_back(native);
        }
        return entries;
    }

    static std::vector<std::string> getSortedDirectoryFiles(
        const std::string& path, const std::string& extension)
    {
        auto entries = getDirectoryFiles(path, extension);
        std::sort(entries.begin(), entries.end());
        return entries;
    }
};