#ifndef TESTALLOCATORACCESSOR_H
#define TESTALLOCATORACCESSOR_H

#include <vector>
#include <array>
#include <cassert>

namespace allocator
{
template<class T, std::size_t N>
class Allocator;
}

template<class T, std::size_t N>
class TestAllocatorAccessor
{
public:
    static std::array<char, N> getMemoryMap(allocator::Allocator<T,N>* alloc)
    {
        std::array<char, N> result;
        char* memStart = static_cast<char*>(alloc->m_memory);
        for(int i = 0; i < N; ++i)
            result[i] = *(memStart+i);
        return result;
    }

    static const T& getValue(allocator::Allocator<T,N>* alloc, size_t pos)
    {
        assert(pos >=0 && pos < N);
        if(static_cast<char*>(alloc->m_memory)[pos] == allocator::DEALLOCATE_STATE)
            throw std::invalid_argument("");
        return static_cast<T*>(alloc->m_valuesBegin)[pos];
    }

    static bool isNull(allocator::Allocator<T,N>* alloc)
    {
        return alloc->m_memory == nullptr;
    }
};

#endif
