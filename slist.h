#ifndef SLIST_H
#define SLIST_H

#include "allocator.h"

namespace slist
{

struct BaseNode
{
    BaseNode* next = nullptr;
};

template<class T>
struct Node: BaseNode
{
    explicit Node(const T& v):
        BaseNode(), value(v){}
    T value;
};

template<class T>
struct SListIterator
{
    typedef std::forward_iterator_tag iterator_category;

    SListIterator() = default;

    explicit SListIterator(BaseNode* node) noexcept
        : node(node) { }

    T& operator*() const noexcept
    {
        return static_cast<Node<T>*>(node)->value;
    }

    T* operator->() const noexcept
    {
        return &(static_cast<Node<T>*>(node)->value);
    }

    SListIterator& operator++() noexcept
    {
        node = node->next;
        return *this;
    }

    SListIterator operator++(int) noexcept
    {
        SListIterator tmp = *this;
        node = node->next;
        return tmp;
    }

    bool operator==(const SListIterator& rhs) const noexcept
    { return node == rhs.node; }

    bool operator!=(const SListIterator& rhs) const noexcept
    { return node != rhs.node; }

    BaseNode* node;
};

template<class T, class Alloc = allocator::Allocator<T,10>>
class SList
{
    using AllocType = typename Alloc::template rebind<Node<T>>::other;
public:
    typedef SListIterator<T> iterator;

    SList() = default;
    ~SList()
    {
        while(m_head)
        {
            auto ptr = m_head;
            m_head = m_head->next;
            m_alloc.destroy(static_cast<Node<T>*>(ptr) );
            m_alloc.deallocate(static_cast<Node<T>*>(ptr), 1);
        }
    }

    template<class... Arg>
    void addItem(Arg... args)
    {
        auto node = m_alloc.allocate(1);
        m_alloc.construct(node, std::forward<Arg>(args)...);
        if(!m_tail)
        {
            m_head = node;
            m_tail = node;
        }
        else
        {
            m_tail->next = node;
            m_tail = node;
        }
    }

    bool isEmpty() const {return !m_head;}

    iterator begin() {return SListIterator<T>(m_head);}
    iterator end() {return SListIterator<T>();}
private:
    BaseNode* m_head = nullptr;
    BaseNode* m_tail = nullptr;
    AllocType m_alloc;
};

}
#endif