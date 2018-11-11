#ifndef SLIST_H
#define SLIST_H

#include <memory>

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

template<class T>
struct ConstSListIterator
{
    typedef std::forward_iterator_tag iterator_category;

    ConstSListIterator() = default;

    explicit ConstSListIterator(BaseNode* node) noexcept
        : node(node) { }

    const T& operator*() const noexcept
    {
        return static_cast<Node<T>*>(node)->value;
    }

    const T* operator->() const noexcept
    {
        return &(static_cast<Node<T>*>(node)->value);
    }

    ConstSListIterator& operator++() noexcept
    {
        node = node->next;
        return *this;
    }

    ConstSListIterator operator++(int) noexcept
    {
        ConstSListIterator tmp = *this;
        node = node->next;
        return tmp;
    }

    bool operator==(const ConstSListIterator& rhs) const noexcept
    { return node == rhs.node; }

    bool operator!=(const ConstSListIterator& rhs) const noexcept
    { return node != rhs.node; }

    BaseNode* node;
};

template<class T, class Alloc = std::allocator<T>>
class SList
{
    using AllocType = typename Alloc::template rebind<Node<T>>::other;
public:
    typedef SListIterator<T> iterator;
    typedef ConstSListIterator<T> const_iterator;

    SList() = default;

    SList(const SList& rhs)
    {
        auto ptr = rhs.m_head;
        while(ptr != nullptr)
        {
            addItem(static_cast<Node<T>*>(ptr)->value);
            ptr = ptr->next;
        }
    }

    template<class U, class AllocatorType>
    SList(const SList<U,AllocatorType>& rhs)
    {
        auto ptr = rhs.m_head;
        while(ptr != nullptr)
        {
            addItem(static_cast<Node<T>*>(ptr)->value);
            ptr = ptr->next;
        }
    }

    SList(SList&& rhs) noexcept
    {
        auto ptr = rhs.m_head;
        while(ptr != nullptr)
        {
            addItem(std::move(static_cast<Node<T>*>(ptr)->value) );
            ptr = ptr->next;
        }
    }

    template<class U, class AllocatorType>
    SList(SList<U,AllocatorType>&& rhs) noexcept
    {
        auto ptr = rhs.m_head;
        while(ptr != nullptr)
        {
            addItem(std::move(static_cast<Node<T>*>(ptr)->value) );
            ptr = ptr->next;
        }
    }

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
    void addItem(Arg&&... args)
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
    const_iterator begin() const {return ConstSListIterator<T>(m_head);}
    const_iterator end() const {return ConstSListIterator<T>();}

    template<class T1, class Alloc1> friend class SList;
private:
    BaseNode* m_head = nullptr;
    BaseNode* m_tail = nullptr;
    AllocType m_alloc;
};

}
#endif
