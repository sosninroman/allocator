#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <cstdlib>
#include <cassert>
#include <new>
#include <algorithm>

template<class T, std::size_t N> class TestAllocatorAccessor;

namespace allocator
{

const char DEALLOCATE_STATE = 0;
const char ALLOCATE_STATE = 1;

template<class T, std::size_t N, bool Expand = false>
class Allocator
{
    struct Chunk
    {
        Chunk* next = nullptr;
        char states[N]; //флаги состояния памяти (свободна или используется)
        char memory[N*sizeof(T)]; //память под объекты
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

    Allocator(Allocator&& rhs) noexcept
    {
        std::swap(rhs.m_head, m_head);
    }

    ~Allocator();

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
    T* expandAndAllocate(size_type n);
    Chunk* findChunk(pointer p);

    template<class T1, std::size_t N1> friend class TestAllocatorAccessor;
    Chunk* m_head = nullptr;
};

template<class T, size_t N, bool Expand>
Allocator<T, N, Expand>::~Allocator()
{
    while(m_head)
    {
        Chunk* ch = m_head;
        m_head = ch->next;
        ch->~Chunk();
        free(ch);
    }
}

template<class T, size_t N, bool Expand>
T* Allocator<T, N, Expand>::expandAndAllocate(size_type n)
{
    void* chkMem = malloc(sizeof(Chunk) );
    if(!chkMem)
        throw std::bad_alloc();
    Chunk* chk = reinterpret_cast<Chunk*>(chkMem);
    new(chk) Chunk();
    for(size_t i = 0; i < N; ++i)
        chk->states[i] = i < n ? ALLOCATE_STATE : DEALLOCATE_STATE;
    chk->next = m_head;
    m_head = chk;
    return reinterpret_cast<T*>(m_head->memory);
}

template<class T, size_t N, bool Expand>
typename Allocator<T, N, Expand>::pointer Allocator<T, N, Expand>::allocate(size_type n, const void*)
{
    if(n > N)
        throw std::bad_alloc();
    if(!m_head)
    {
        return expandAndAllocate(n);
    }
    else
    {
        auto chk = m_head;
        while(chk)
        {
            char* lptr = chk->states;
            auto rptr = lptr;
            auto end = lptr+N;
            while(rptr != end)
            {
                if(*rptr == ALLOCATE_STATE)
                    lptr = ++rptr;
                else
                    if(static_cast<size_t>(++rptr-lptr) == n)
                    {
                        auto result = reinterpret_cast<T*>(chk->memory) + (lptr - chk->states);
                        while(lptr != rptr)
                            *lptr++ = ALLOCATE_STATE;
                        return result;
                    }
            }
            chk = chk->next;
        }
        if(Expand)
        {
            return expandAndAllocate(n);
        }
        else
        {
            throw std::bad_alloc();
        }
    }
}

template<class T, size_t N, bool Expand>
typename Allocator<T, N, Expand>::Chunk* Allocator<T, N, Expand>::findChunk(pointer p)
{
    Chunk* chk = m_head;
    while(chk)
    {
        pointer ptr = reinterpret_cast<pointer>(chk->memory);
        for(size_t i = 0; i < N; ++i)
        {
            if(ptr == p)
                return chk;
            ++ptr;
        }
        chk = chk->next;
    }
    return nullptr;
}

template<class T, size_t N, bool Expand>
void Allocator<T, N, Expand>::deallocate(pointer p, size_type n)
{
    Chunk* chk = findChunk(p);
    assert(chk);
    auto shift = p - reinterpret_cast<pointer>(chk->memory);
    char* lptr = chk->states + shift;
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
