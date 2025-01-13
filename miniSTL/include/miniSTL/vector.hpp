#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <limits>
#include <stdexcept>
#include <utility>
#include <compare>
#include <initializer_list>

/*
? 定义于头文件 <memory>
? template< class T >
? struct allocator;
*/
//?                             分配器 allocator  目的：封装STL容器在内存管理上的低层细节
//?   T 为存储的对象类型
//?   Alloc 为使用的分配器, 并默认使用 std::allocator 作为对象的分配器
template <class T, class Alloc = std::allocator<T>>
struct Vector
{
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    //? ptrdiff_t类型变量通常用来保存两个指针减法操作的结果
    using pointer = T *;
    using const_pointer = T const *;
    //? 指针本身是常量    注意与 常量指针 区分(内容是const,指针不是const)
    using reference = T &;
    using const_reference = T const &;
    using iterator = T *;
    using const_iterator = T const *;
    using reverse_iterator = std::reverse_iterator<T *>;
    using const_reverse_iterator = std::reverse_iterator<T const *>;

    T *m_data;
    size_t m_size;
    size_t m_cap;
    [[no_unique_address]] Alloc m_alloc;
    //?   地址 无须 和其类的所有其他非静态数据成员的地址不同
    //?      若该成员拥有空类型（例如无状态分配器），则编译器可将它优化为不占空间
    Vector()
    {
        m_data = NULL;
        m_size = 0;
        m_cap = 0;
    }

    Vector(std::initializer_list<T> ilist, Alloc const &alloc = Alloc())
        : Vector(ilist.begin(), ilist.end(), alloc) {} //* 利用初始化列表构造啊

    explicit Vector(size_t n, Alloc const &alloc = Alloc()) //* 无默认值构造
        : m_alloc(alloc)
    {
        m_data = m_alloc.allocate(n);
        m_size = n;
        m_cap = n;
        for (size_t i = 0; i != n; i++)
            std::__construct_at(&m_data[i]);
    }

    Vector(size_t n, T const &val, Alloc const &alloc = Alloc()) //* 自定义默认值构造
        : m_alloc(alloc)
    {
        m_data = m_alloc.allocate(n);
        m_size = n;
        m_cap = n;
        for (size_t i = 0; i != n; i++)
            std::__construct_at(&m_data[i], val);
    }

    template <std::random_access_iterator InputIt>
    Vector(InputIt first, InputIt last, Alloc const &alloc = Alloc()) //* 读取input区构造
        : m_alloc(alloc)
    {
        size_t n = last - first;
        m_data = m_alloc.allocate(n);
        m_size = n;
        m_cap = n;
        for (size_t i = 0; i != n; i++)
        {
            std::__construct_at(&m_data[i], *first);
            first++;
        }
    }

    void clear()
    {
        for (size_t i = 0; i != m_size; i++)
            std::destroy_at(&m_data[i]);
        m_size = 0;
    }

    void resize(size_t n) //*无值重构
    {
        if (n < m_size)
            for (size_t i = n; i != m_size; i++)
                std::destroy_at(&m_data[i]);
        else if (n > m_size)
        {
            reserve(n);
            for (size_t i = m_size; i != n; i++)
                std::__construct_at(&m_data[i]);
        }
        m_size = n;
    }

    void resize(size_t n, T const &val) //*带值重构
    {
        if (n < m_size)
            for (size_t i = n; i != m_size; i++)
                std::destroy_at(&m_data[i]);
        else if (n > m_size)
        {
            reserve(n);
            for (size_t i = m_size; i != n; i++)
                std::__construct_at(&m_data[i], val);
        }
        m_size = n;
    }

    void shrink_to_fit()
    {
        auto old_data = m_data;
        auto old_cap = m_cap;
        m_cap = m_size;
        if (m_size == 0)
            m_data = NULL;
        else
            m_data = m_alloc.allocate(m_size);
        if (old_cap != 0) [[likely]]
        { //? ‌向编译器提供关于代码分支执行概率的提示，帮助编译器进行更好的优化。
            for (size_t i = 0; i != m_size; i++)
            {
                std::__construct_at(&m_data[i], std::move_if_noexcept(old_data[i]));
                //?                               若移动构造函数不抛出异常则获得右值引用
                std::destroy_at(&old_data[i]);
            }
            m_alloc.deallocate(old_data, old_cap);
            //?  释放之前通过 allocate 方法分配的内存
        }
    }

    void reserve(size_t n)
    { //? 与realloc类似  可以一次性分配指定大小的内存
        if (n <= m_cap)
            return;
        auto old_data = m_data;
        auto old_cap = m_cap;
        if (n == 0)
        {
            m_data = NULL;
            m_cap = 0;
        }
        else
        {
            m_data = m_alloc.allocate(n);
            m_cap = n;
        }
        if (old_cap != 0)
        {
            for (size_t i = 0; i != m_size; i++)
                std::__construct_at(&m_data[i], std::move_if_noexcept(old_data[i]));
            for (size_t i = 0; i != m_size; i++)
                std::destroy_at(&old_data[i]);
            m_alloc.deallocate(old_data, old_cap);
        }
    }

    size_t capacity() const
    {
        return m_cap;
    }

    size_t size() const
    {
        return m_size;
    }

    bool empty() const
    {
        return m_size == 0;
    }

    T const &operator[](size_t i) const
    {
        return m_data[i];
    }

    T &operator[](size_t i)
    {
        return m_data[i];
    }

    T const &at(size_t i) const
    {
        if (i >= m_size) [[unlikely]]
            throw std::out_of_range("vector::at");
        return m_data[i];
    }

    T &at(size_t i)
    {
        if (i >= m_size) [[unlikely]]
            throw std::out_of_range("vector::at");
        return m_data[i];
    }

    Vector(Vector &&that)
        : m_alloc(std::move(that.m_alloc))
    {
        m_data = that.m_data;
        m_size = that.m_size;
        m_cap = that.m_cap;
        that.m_data = NULL;
        that.m_size = 0;
        that.m_cap = 0;
    }

    Vector(Vector &&that, Alloc const &alloc)
        : m_alloc(alloc)
    {
        m_data = that.m_data;
        m_size = that.m_size;
        m_cap = that.m_cap;
        that.m_data = NULL;
        that.m_size = 0;
        that.m_cap = 0;
    }

    Vector &operator=(Vector &&that) const
    {
        if (&that == this) [[unlikely]]
            return *this;
        for (size_t i = 0; i != m_size; i++)
            std::destroy_at(&m_data[i]);
        if (m_cap != 0)
            m_alloc.deallocate(m_data, m_cap);

        m_data = that.m_data;
        m_size = that.m_size;
        m_cap = that.m_cap;
        that.m_data = NULL;
        that.m_size = 0;
        that.m_cap = 0;
        return *this;
    }

    void swap(Vector &that)
    {
        std::swap(m_data, that.m_data);
        std::swap(m_size, that.m_size);
        std::swap(m_cap, that.m_cap);
        std::swap(m_alloc, that.m_alloc);
    }

    Vector(Vector const &that)
        : m_alloc(that.m_alloc)
    {
        m_size = that.m_size;
        m_cap = that.m_size;
        if (m_size != 0)
        {
            m_data = m_alloc.allocate(m_size);
            for (size_t i = 0; i != m_size; i++)
                std::__construct_at(&m_data[i], std::as_const(that.m_data[i]));
        }
        else
            m_data = NULL;
    }

    Vector(Vector const &that, Alloc const &alloc) : m_alloc(alloc)
    {
        m_size = that.m_size;
        m_cap = that.m_size;
        if (m_size != 0)
        {
            m_data = m_alloc.allocate(m_size);
            for (size_t i = 0; i != m_size; i++)
                std::__construct_at(&m_data[i], std::as_const(that.m_data[i]));
        }
        else
            m_data = NULL;
    }

    Vector &operator=(Vector const &that)
    {
        if (&that == this) [[unlikely]]
            return *this;
        reserve(that.m_size);
        m_size = that.m_size;
        for (size_t i = 0; i != m_size; i++)
            std::__construct_at(&m_data[i], std::as_const(that.m_data[i]));
        return *this;
    }

    T const &front() const
    {
        return *m_data;
    }

    T &front()
    {
        return *m_data;
    }

    T const &back() const
    {
        return m_data[m_size - 1];
    }

    T &back()
    {
        return m_data[m_size - 1];
    }

    void push_back(T const &val)
    {
        if (m_size + 1 >= m_cap) [[unlikely]]
            reserve(m_size + 1);
        std::__construct_at(&m_data[m_size], val);
        m_size = m_size + 1;
    }

    void push_back(T &&val)
    {
        if (m_size + 1 >= m_cap) [[unlikely]]
            reserve(m_size + 1);
        std::__construct_at(&m_data[m_size], std::move(val));
        m_size = m_size + 1;
    }

    T *data()
    {
        return m_data;
    }

    T const *data() const
    {
        return m_data;
    }

    T const *cdata() const
    {
        return m_data;
    }

    T *begin()
    {
        return m_data;
    }

    T *end()
    {
        return m_data + m_size;
    }

    T const *begin() const
    {
        return m_data;
    }

    T const *end() const
    {
        return m_data + m_size;
    }

    T const *cbegin() const
    {
        return m_data;
    }

    T const *cend() const
    {
        return m_data + m_size;
    }

    std::reverse_iterator<T *> rbegin()
    {
        return std::make_reverse_iterator(m_data + m_size);
    }

    std::reverse_iterator<T *> rend()
    {
        return std::make_reverse_iterator(m_data);
    }

    std::reverse_iterator<T const *> crbegin() const
    {
        return std::make_reverse_iterator(m_data + m_size);
    }

    std::reverse_iterator<T const *> crend() const
    {
        return std::make_reverse_iterator(m_data);
    }

    void pop_back()
    {
        m_size--;
        std::destroy_at(&m_data[m_size]);
    }

    T *erase(T const *it) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        size_t i = it - m_data;
        for (size_t j = i + 1; j != m_size; j++)
            m_data[j - 1] = std::move(m_data[j]);
        m_size--;
        std::destroy_at(&m_data[m_size]);
        return const_cast<T *>(it);
    }

    T *erase(T const *first, T const *last) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        size_t diff = last - first;
        for (size_t j = last - m_data; j != m_size; j++)
            m_data[j - diff] = std::move(m_data[j]);
        m_size -= diff;
        for (size_t j = m_size; j != m_size + diff; j++)
            std::destroy_at(&m_data[j]);
        return const_cast<T *>(first);
    }

    void assign(size_t n, T const &val)
    {
        clear();
        reserve(n);
        m_size = n;
        for (size_t i = 0; i != n; i++)
            std::__construct_at(&m_data[i], val);
    }

    template <std::random_access_iterator InputIt>
    void assign(InputIt first, InputIt last)
    {
        clear();
        size_t n = last - first;
        reserve(n);
        m_size = n;
        for (size_t i = 0; i != n; i++)
        {
            std::__construct_at(m_data[i], *first);
            first++;
        }
    }

    void assign(std::initializer_list<T> ilist)
    {
        assign(ilist.begin(), ilist.end());
    }

    Vector &operator=(std::initializer_list<T> ilist)
    {
        assign(ilist.begin(), ilist.end());
    }

    T *insert(T const *it, T &&val)
    {
        size_t j = it - m_data;
        reserve(m_size + 1);
        for (size_t i = m_size; i != j; i--)
        {
            std::__construct_at(&m_data[i], std::move(m_data[i - 1]));
            std::destroy_at(&m_data[i - 1]);
        }
        m_size++;
        std::__construct_at(&m_data[j], std::move(val));
        return m_data + j;
    }

    T *insert(T const *it, T const &val)
    {
        size_t j = it - m_data;
        reserve(m_size + 1);
        for (size_t i = m_size; i != j; i--)
        {
            std::__construct_at(&m_data[i], std::move(m_data[i - 1]));
            std::destroy_at(&m_data[i - 1]);
        }
        m_size++;
        std::__construct_at(&m_data[j], val);
        return m_data + j;
    }

    T *insert(T const *it, size_t n, T const &val);

    template <std::random_access_iterator InputIt>
    T *insert(T const *it, InputIt first, InputIt last)
    {
        size_t j = it - m_data;
        size_t n = last - first;
        if (n == 0) [[unlikely]]
            return const_cast<T *>(it);
        reserve(m_size + n);
        for (size_t i = m_size; i != j; i--)
        {
            std::__construct_at(&m_data[i + n - 1], std::move(m_data[i - 1]));
            std::destroy_at(&m_data[i - 1]);
        }
        m_size += n;
        for (size_t i = j; i != j + n; i++)
        {
            std::__construct_at(&m_data[i], *first);
            first++;
        }
        return m_data + j;
    }

    T *insert(T const *it, std::initializer_list<T> ilist)
    {
        return insert(it, ilist.begin(), ilist.end());
    }

    ~Vector()
    {
        for (size_t i = 0; i != m_size; i++)
            std::destroy_at(&m_data[i]);
        if (m_cap != 0)
            m_alloc.deallocate(m_data, m_cap);
    }

    bool operator==(Vector const &that) const
    {
        if (m_size != that.m_size)
            return false;
        for (size_t i = 0; i < m_size; ++i)
            if (!(m_data[i] == that.m_data[i]))
                return false;
        return true;
    }
};
