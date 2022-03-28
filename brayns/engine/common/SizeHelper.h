#pragma once

#include <memory>
#include <vector>

namespace brayns
{
struct SizeHelper
{
    template<typename T>
    static size_t vectorSize(const std::vector<T> &vector)
    {
        return sizeof(T) * vector.capacity();
    }

    template<typename T>
    static size_t vectorSize(const std::vector<std::unique_ptr<T>> &vector)
    {
        return (sizeof(T) + sizeof(std::unique_ptr<T>)) * vector.capacity();
    }

    template<typename T>
    static size_t vectorSize(const std::vector<std::shared_ptr<T>> &vector)
    {
        return (sizeof(T) + sizeof(std::shared_ptr<T>)) * vector.capacity();
    }
};
}
