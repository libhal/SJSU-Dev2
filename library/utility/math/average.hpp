// Average value returned can only go up to 7 numbers.
// Please ensure your average value does not exceed this limit.

#pragma once

#include <initializer_list>
#include <iterator>

template<typename T>
constexpr float Average(const T * array, size_t size)
{
    float sum = 0;
    float length = static_cast<float>(size);

    for (size_t i = 0; i < size; i++)
    {
        sum += static_cast<float>(array[i]);
    }

    return sum/length;
}
template<typename T>
constexpr float Average(std::initializer_list<T> array)
{
    return Average(array.begin(), array.size());
}

template<typename T, size_t size>
constexpr float Average(const T (&array)[size])
{
    return Average(array, size);
}
