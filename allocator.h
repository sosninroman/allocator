#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <new>
#include <cstdlib>
#include <cassert>
#include <iterator>

template<class T, std::size_t N> class TestAllocatorAccessor;

namespace allocator
{


const char DEALLOCATE_STATE = 0;
const char ALLOCATE_STATE = 1;

template<class ValueType, std::size_t N>
class Allocator
{
public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = ValueType*;
    using const_pointer = const ValueType*;
    using reference = ValueType&;
    using const_reference = const ValueType&;
    using value_type = ValueType;

    template<class U>
    struct rebind
    {
        typedef Allocator<U, N> other;
    };

    Allocator() noexcept = default;
    Allocator(const Allocator&) noexcept {}
    template <class U>
    Allocator(const Allocator<U, N>&) noexcept
    {}

    ~Allocator();


    pointer address(reference val) const noexcept
    {
        return std::addressof(val);
    }

    const_pointer address(const_reference val) const noexcept
    {
        return std::addressof(val);
    }

    pointer allocate(size_type n, const void* = 0);

    void deallocate(pointer p, size_type n);

    size_type max_size() const noexcept
    {
        return N;
    }

    template<class U, class... Args>
    void construct(U* p, Args&&... args)
    {
        ::new((void *)p) U(std::forward<Args>(args)...);
    }

    template<class U>
    void destroy(U* p)
    {
        p->~U();
    }

private:
    template<class T, std::size_t N1> friend class TestAllocatorAccessor;
    void* m_memory = nullptr;
    void* m_objectsStates = nullptr;
};

template<class T, size_t N>
Allocator<T, N>::~Allocator()
{
    if(m_memory)
        free(m_memory);
    if(m_objectsStates)
        free(m_objectsStates);
}

template<class T, size_t N>
typename Allocator<T, N>::pointer Allocator<T, N>::allocate(size_type n, const void*)
{
    if(n > N)
        throw std::bad_alloc();
    if(!m_memory)
    {
        m_memory = malloc(N * sizeof(T) );
        assert(!m_objectsStates);
        m_objectsStates = malloc(N);
        auto ptr = static_cast<char*>(m_objectsStates);
        for(int i = 0; i < N; ++i)
            *(ptr++) = i < n ? ALLOCATE_STATE : DEALLOCATE_STATE;
        return static_cast<pointer>(m_memory);
    }
    else
    {
        auto lptr = static_cast<char*>(m_objectsStates);
        auto rptr = static_cast<char*>(m_objectsStates);
        auto end = static_cast<char*>(m_objectsStates) + N;
        while(rptr != end)
        {
            if(*rptr == ALLOCATE_STATE)
                lptr = ++rptr;
            else
                if(++rptr-lptr == n)
                {
                    auto result = static_cast<T*>(m_memory) +
                            (lptr - static_cast<char*>(m_objectsStates) );
                    while(lptr != rptr)
                        *lptr++ = ALLOCATE_STATE;
                    return result;
                }
        }
        throw std::bad_alloc();
    }
}

template<class T, size_t N>
void Allocator<T, N>::deallocate(pointer p, size_type n)
{
    auto shift = p - static_cast<T*>(m_memory);
    char* lptr = static_cast<char*>(m_objectsStates) + shift;
    char* rptr = lptr + n;
    while(lptr != rptr)
        *lptr++ = DEALLOCATE_STATE;
}

template<class T, std::size_t N>
  inline bool
  operator==(const allocator::Allocator<T,N>&, const allocator::Allocator<T,N>&)
  { return true; }

template<class T, std::size_t N>
  inline bool
  operator!=(const allocator::Allocator<T,N>&, const allocator::Allocator<T,N>&)
  { return false; }

}

#endif
