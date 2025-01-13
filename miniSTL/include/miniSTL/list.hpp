#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <limits>
#include <stdexcept>
#include <utility>
#include <compare>
#include <initializer_list>

template <class T>
struct ListBaseNode
{
    ListBaseNode *m_next;
    ListBaseNode *m_prev;

    inline T &value();
    inline T const &value() const;
};

template <class T>
struct ListValueNode : ListBaseNode<T>
{
    union
    {
        T m_value;
    };
};

template <class T>
inline T &ListBaseNode<T>::value()
{
    return static_cast<ListValueNode<T> &>(*this).m_value;
}

template <class T>
inline T const &ListBaseNode<T>::value() const
{
    return static_cast<ListValueNode<T> const &>(*this).m_value;
}

template <class T, class Alloc = std::allocator<T>>
struct List
{
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = T const *;
    using reference = T &;
    using const_reference = T const &;

private:
    using ListNode = ListBaseNode<T>;
    using AllocNode = std::allocator_traits<Alloc>::template rebind_alloc<ListValueNode<T>>;

    ListNode m_dummy;
    size_t m_size;
    [[no_unique_address]] Alloc m_alloc;

    ListNode *newNode()
    {
        return AllocNode{m_alloc}.allocate(1);
    }

    void deleteNode(ListNode *node) noexcept
    {
        AllocNode{m_alloc}.deallocate(static_cast<ListValueNode<T> *>(node), 1);
    }

private:
    void _uninit_move_assign(List &&that)
    {
        auto prev = that.m_dummy.m_prev;
        auto next = that.m_dummy.m_next;
        prev->m_next = &m_dummy;
        next->m_prev = &m_dummy;
        m_dummy = that.m_dummy;
        that.m_dummy.m_prev = that.m_dummy.m_next = &that.m_dummy;
        m_size = that.m_size;
        that.m_size = 0;
    }

private:
    template <std::input_iterator InputIt>
    void _uninit_assign(InputIt first, InputIt last)
    {
        m_size = 0;
        ListNode *prev = &m_dummy;
        while (first != last)
        {
            ListNode *node = newNode();
            prev->m_next = node;
            node->m_prev = prev;
            std::__construct_at(&node->value(), *first);
            prev = node;
            ++first;
            ++m_size;
        }
        m_dummy.m_prev = prev;
        prev->m_next = &m_dummy;
    }

    void _uninit_assign(size_t n, T const &val)
    {
        ListNode *prev = &m_dummy;
        while (n)
        {
            ListNode *node = newNode();
            prev->m_next = node;
            node->m_prev = prev;
            std::__construct_at(&node->value(), val);
            prev = node;
            --n;
        }
        m_dummy.m_prev = prev;
        prev->m_next = &m_dummy;
        m_size = n;
    }

    void _uninit_assign(size_t n)
    {
        ListNode *prev = &m_dummy;
        while (n)
        {
            ListNode *node = newNode();
            prev->m_next = node;
            node->m_prev = prev;
            std::__construct_at(&node->value());
            prev = node;
            --n;
        }
        m_dummy.m_prev = prev;
        prev->m_next = &m_dummy;
        m_size = n;
    }

public:
    template <class... Args>
    T &emplace_back(Args &&...args)
    {
        ListNode *node = newNode();
        ListNode *prev = m_dummy.m_prev;
        prev->m_next = node;
        node->m_prev = prev;
        node->m_next = &m_dummy;
        std::__construct_at(&node->value(), std::forward<Args>(args)...);
        m_dummy.m_prev = node;
        ++m_size;
        return node->value();
    }

    template <class... Args>
    T &emplace_front(Args &&...args)
    {
        ListNode *node = newNode();
        ListNode *next = m_dummy.m_next;
        next->m_prev = node;
        node->m_next = next;
        node->m_prev = &m_dummy;
        std::__construct_at(&node->value(), std::forward<Args>(args)...);
        m_dummy.m_next = node;
        ++m_size;
        return node->value();
    }

public:
    List()
    {
        m_size = 0;
        m_dummy.m_next = &m_dummy;
        m_dummy.m_prev = &m_dummy;
    }

    explicit List(Alloc const &alloc) noexcept : m_alloc(alloc)
    {
        m_size = 0;
        m_dummy.m_next = &m_dummy;
        m_dummy.m_prev = &m_dummy;
    }

    List(List &&that) : m_alloc(std::move(that.m_alloc))
    {
        _uninit_move_assign(std::move(that));
    }

    List(List &&that, Alloc const &alloc) : m_alloc(alloc)
    {
        _uninit_move_assign(std::move(that));
    }

    List(List const &that) : m_alloc(that.m_alloc)
    {
        _uninit_assign(that.cbegin(), that.cend());
    }

    List(List const &that, Alloc const &alloc) : m_alloc(alloc)
    {
        _uninit_assign(that.cbegin(), that.cend());
    }

    List &operator=(List const &that)
    {
        assign(that.cbegin(), that.cend());
    }

    bool empty()
    {
        return m_dummy.m_prev == m_dummy.m_next;
    }

    T &front()
    {
        return m_dummy.m_next->value();
    }

    T &back()
    {
        return m_dummy.m_prev->value();
    }

    T const &front() const
    {
        return m_dummy.m_next->value();
    }

    T const &back() const
    {
        return m_dummy.m_prev->value();
    }

    explicit List(size_t n, Alloc const &alloc = Alloc())
        : m_alloc(alloc)
    {
        _uninit_assign(n);
    }

    List(size_t n, T const &val, Alloc const &alloc = Alloc()) : m_alloc(alloc)
    {
        _uninit_assign(n, val);
    }

    template <std::input_iterator InputIt>
    List(InputIt first, InputIt last, Alloc const &alloc = Alloc())
    {
        _uninit_assign(first, last);
    }

    List(std::initializer_list<T> ilist, Alloc const &alloc = Alloc())
        : List(ilist.begin(), ilist.end(), alloc) {}

    List &operator=(std::initializer_list<T> ilist)
    {
        assign(ilist);
    }

    size_t size() const
    {
        return m_size;
    }

    template <std::input_iterator InputIt>
    void assign(InputIt first, InputIt last)
    {
        clear();
        _uninit_assign(first, last);
    }

    void assign(std::initializer_list<T> ilist)
    {
        clear();
        _uninit_assign(ilist.begin(), ilist.end());
    }

    void assign(size_t n, T const &val)
    {
        clear();
        _uninit_assign(n, val);
    }

    void push_back(T const &val)
    {
        emplace_back(val);
    }

    void push_back(T &&val)
    {
        emplace_back(std::move(val));
    }

    void push_front(T const &val)
    {
        emplace_front(val);
    }

    void push_front(T &&val)
    {
        emplace_front(std::move(val));
    }

    ~List()
    {
        clear();
    }

    void clear()
    {
        ListNode *curr = m_dummy.m_next;
        while (curr != &m_dummy)
        {
            std::destroy_at(&curr->value());
            auto next = curr->m_next;
            deleteNode(curr);
            curr = next;
        }
        m_dummy.m_next = &m_dummy;
        m_dummy.m_prev = &m_dummy;
        m_size = 0;
    }

    struct iterator
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;

    private:
        ListNode *m_curr;

        friend List;

        explicit iterator(ListNode *curr) : m_curr(curr) {}

    public:
        iterator() = default;

        iterator &operator++() //* ++iterator 先加后用
        {
            m_curr = m_curr->m_next;
            return *this;
        }

        iterator operator++(int) //* iterator++ 先用后加
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        iterator &operator--() //* --iterator  先减后用
        {
            m_curr = m_curr->m_prev;
            return *this;
        }

        iterator operator--(int) //* iterator-- 先用后减
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        T &operator*() const
        {
            return m_curr->value();
        }

        bool operator!=(iterator const &that) const
        {
            return m_curr != that.m_curr;
        }

        bool operator==(iterator const &that) const
        {
            return !(*this != that);
        }
    };

    struct const_iterator
    {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T const *;
        using reference = T const &;

    private:
        ListNode const *m_curr;

        friend List;

        explicit const_iterator(ListNode const *curr)
            : m_curr(curr) {}

    public:
        const_iterator() = default;

        const_iterator(iterator that)
            : m_curr(that.m_curr)
        {
        }

        explicit operator iterator()
        {
            return iterator{const_cast<ListNode *>(m_curr)};
        }

        const_iterator &operator++() //* ++iterator  先加后用
        {
            m_curr = m_curr->m_next;
            return *this;
        }

        const_iterator operator++(int) //* iterator++ 先用后加
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        const_iterator &operator--() //* --iterator   先减后用
        {
            m_curr = m_curr->m_prev;
            return *this;
        }

        const_iterator operator--(int) //* iterator--  先用后减
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        T const &operator*() const
        {
            return m_curr->value();
        }

        bool operator!=(const_iterator const &that) const
        {
            return m_curr != that.m_curr;
        }

        bool operator==(const_iterator const &that) const
        {
            return !(*this != that);
        }
    };

    iterator begin()
    {
        return iterator{m_dummy.m_next};
    }

    iterator end()
    {
        return iterator{&m_dummy};
    }

    const_iterator cbegin() const
    {
        return const_iterator{m_dummy.m_next};
    }

    const_iterator cend() const
    {
        return const_iterator{&m_dummy};
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }

    using reverse_iterator = std::reverse_iterator<iterator>;
    using reverse_const_iterator = std::reverse_iterator<const_iterator>;

    reverse_iterator rbegin()
    {
        return std::make_reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return std::make_reverse_iterator(begin());
    }

    reverse_const_iterator crbegin() const
    {
        return std::make_reverse_iterator(cend());
    }

    reverse_const_iterator crend() const
    {
        return std::make_reverse_iterator(cbegin());
    }

    reverse_const_iterator rbegin() const
    {
        return crbegin();
    }

    reverse_const_iterator rend() const
    {
        return crend();
    }

    iterator erase(const_iterator pos)
    {
        ListNode *node = const_cast<ListNode *>(pos.m_curr);
        auto next = node->m_next;
        auto prev = node->m_prev;
        prev->m_next = next;
        next->m_prev = prev;
        std::destroy_at(&node->value());
        deleteNode(node);
        --m_size;
        return iterator{next};
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        while (first != last)
        {
            first = erase(first);
        }
        return iterator(first);
    }

    void pop_front()
    {
        erase(begin());
    }

    void pop_back()
    {
        erase(std::prev(end()));
    }

    size_t remove(T const &val)
    {
        auto first = begin();
        auto last = begin();
        size_t count = 0;
        while (first != last)
        {
            if (*first == val)
            {
                first = erase(first);
                count++;
            }
            else
                first++;
        }
        return count;
    }

    template <class... Args>
    iterator emplace(const_iterator pos, Args &&...args)
    {
        ListNode *curr = newNode();
        ListNode *next = const_cast<ListNode *>(pos.m_curr);
        ListNode *prev = next->m_prev;
        curr->m_prev = prev;
        prev->m_next = curr;
        curr->m_next = next;
        next->m_prev = curr;
        std::__construct_at(&curr->value(), std::forward<Args>(args)...);
        m_size++;
        return iterator{curr};
    }

    iterator insert(const_iterator pos, const T &val)
    {
        return emplace(pos, val);
    }

    iterator insert(const_iterator pos, T &&val)
    {
        return emplace(pos, std::move(val));
    }

    iterator insert(const_iterator pos, size_t n, T const &val)
    {
        auto orig = pos;
        bool had_orig = false;
        while (n)
        {
            pos = emplace(pos, val);
            if (!had_orig)
            {
                had_orig = true;
                orig = pos;
            }
            pos++;
            n--;
        }
        return iterator(orig);
    }

    template <std::input_iterator InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        auto orig = pos;
        bool had_orig = false;
        while (first != last)
        {
            pos = emplace(pos, *first);
            if (!had_orig)
            {
                had_orig = true;
                orig = pos;
            }
            pos++;
            first++;
        }
        return iterator(orig);
    }

    iterator insert(const_iterator pos, std::initializer_list<T> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }

    bool operator==(List const &that) const noexcept
    {
        if (m_size != that.m_size)
            return false;

        const_iterator it1(this->m_dummy.m_next);
        const_iterator it2(that.m_dummy.m_next);
        const_iterator end1(&this->m_dummy);
        const_iterator end2(&that.m_dummy);
        while (it1 != end1 && it2 != end2)
        {
            if (!(*it1 == *it2))
                return false;
            it1++;
            it2++;
        }

        return true;
    }
};
