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

    static std::string getExtensionWithDot(const std::string& extension)
    {
        if (extension.empty() || extension[0] == '.')
        {
            return extension;
        }
        return "." + extension;
    }

    static std::vector<std::string> getDirectoryFiles(
        const std::string& path, const std::string& extension)
    {
        auto extensionWithDot = getExtensionWithDot(extension);
        std::vector<std::string> entries;
        for (const auto& entry : DirectoryIterator(path))
        {
            if (!boost::filesystem::is_regular_file(entry))
            {
                continue;
            }
            auto& path = entry.path();
            if (path.extension() != extensionWithDot)
            {
                continue;
            }
            entries.push_back(path.native());
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