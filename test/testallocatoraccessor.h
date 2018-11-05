#ifndef TESTALLOCATORACCESSOR_H
#define TESTALLOCATORACCESSOR_H

#include <vector>
#include <array>
#include <cassert>

namespace allocator
{
template<class T, std::size_t N, bool Expand>
class Allocator;
}

template<class T, std::size_t N>
class TestAllocatorAccessor
{
public:
    static std::array<char, N> getMemoryMap(allocator::Allocator<T,N>* alloc)
    {
        std::array<char, N> result;
        char* memStart = static_cast<char*>(alloc->m_head->states);
        for(int i = 0; i < N; ++i)
            result[i] = *(memStart+i);
        return result;
    }

    static const T& getValue(allocator::Allocator<T,N>* alloc, size_t pos)
    {
        assert(pos >=0 && pos < N);
        if(static_cast<char*>(alloc->m_head->states)[pos] == allocator::DEALLOCATE_STATE)
            throw std::invalid_argument("");
        return *(reinterpret_cast<T*>(alloc->m_head->memory) + pos);
    }

    static bool isNull(allocator::Allocator<T,N>* alloc)
    {
        return alloc->m_head == nullptr;
    }
};

#endif
