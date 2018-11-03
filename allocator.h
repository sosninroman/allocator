#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <new>
#include <cstdlib>
#include <cassert>
#include <iterator>

namespace allocator
{

const char DEALLOCATE_STATE = 0;
const char ALLOCATE_STATE = 1;

template<class T, std::size_t N, bool Expand = false>
class Allocator
{
//    template<class U, class Sz>
    struct Chunk
    {
        void* valuesBegin() {return static_cast<void*>(&memory[N]);}
        Chunk* next = nullptr;
        char memory[N * (sizeof(T)+1)];
    };
public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    template<class U>
    struct rebind
    {
        typedef Allocator<U, N, Expand> other;
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
    void expand(size_type n) noexcept;

    template<class T1, std::size_t N1> friend class TestAllocatorAccessor;
//    void* m_memory = nullptr;
//    void* m_valuesBegin = nullptr;
    Chunk* m_head = nullptr;
};

template<class T, size_t N, bool Expand>
Allocator<T, N, Expand>::~Allocator()
{
//    if(m_memory)
//        free(m_memory);
    while(m_head)
    {
        Chunk* ch = m_head;
        m_head = ch->next;
        free(ch);
    }
}

template<class T, size_t N, bool Expand>
void Allocator<T, N, Expand>::expand(size_type n) noexcept
{
    Chunk* chk = static_cast<Chunk*>(malloc(sizeof(Chunk) ) );
    for(int i = 0; i < N; ++i)
        chk->memory[i] = i < n ? ALLOCATE_STATE : DEALLOCATE_STATE;
    chk->next = m_head;
    m_head = chk;
}

template<class T, size_t N, bool Expand>
typename Allocator<T, N, Expand>::pointer Allocator<T, N, Expand>::allocate(size_type n, const void*)
{
    if(n > N)
        throw std::bad_alloc();
//    if(!m_memory)
    if(!m_head)
    {
        expand(n);
        return static_cast<T*>(m_head->valuesBegin() );
    }
    else
    {
        auto chk = m_head;
        while(chk)
        {
            auto lptr = static_cast<char*>(chk->memory);
            auto rptr = lptr;
            auto end = static_cast<char*>(chk->valuesBegin() );
            while(rptr != end)
            {
                if(*rptr == ALLOCATE_STATE)
                    lptr = ++rptr;
                else
                    if(++rptr-lptr == n)
                    {
                        auto result = static_cast<T*>(chk->valuesBegin() ) +
                                (lptr - static_cast<char*>(chk->memory) );
                        while(lptr != rptr)
                            *lptr++ = ALLOCATE_STATE;
                        return result;
                    }
            }
            chk = chk->next;
        }
        if(Expand)
        {
            expand(n);
            return static_cast<T*>(m_head->valuesBegin() );
        }
        else
        {
            throw std::bad_alloc();
        }
    }
}

template<class T, size_t N, bool Expand>
void Allocator<T, N, Expand>::deallocate(pointer p, size_type n)
{
    Chunk* chk = m_head;
    while(chk)
    {
        pointer ptr = static_cast<pointer>(chk->valuesBegin() );
        for(int i = 0; i < N; ++i)
        {
            if(ptr == p)
                break;
            ++ptr;
        }
        if(ptr==p)
            break;
        chk = chk->next;
    }
    assert(chk);
    auto shift = p - static_cast<T*>(chk->valuesBegin() );
    char* lptr = static_cast<char*>(chk->memory) + shift;
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
