#ifndef CESA_VECTOR_HPP
#define CESA_VECTOR_HPP

/**
 * vector.hpp
 *
 * A fixed-size vector with stack-allocated memory.
 *
 * The cesa::vector class provides an interface and behavior similar to that of std::vector
 * but uses static heap allocation. This introduces the limitation of defining a maximum number of
 * elements the vector will ever need. It is recommended to use cesa::vector over std::vector
 * in cases where dynamic heap allocation is undesired.
 *
 * Note:
 * As the underlying data is stored on the stack, one should avoid using cesa::vector in cases
 * where a large number of elements are needed, as it could lead to a stack overflow. Consider your
 * target platform's stack size and compare it to the byte usage of cesa::vector
 * (max_elements * sizeof(T)).
 *
 * Attention:
 * Iterator Invalidation:
 * Iterators are invalidated by any operation that changes the size of the vector
 * (e.g., insertion, erasure, assignment to a smaller size).
 * Iterators to elements after the insertion/erasure point are invalidated.
 * Therefore, if element erasure is required while iterating, a reverse iterator is recommended.
 */

#include <new>
#include <cstddef>
#include <iostream>
#include <iterator>

namespace cesa
{
    template <typename T, std::size_t max_elements>
    class vector
    {
    public:
        using value_type             = T;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using reference              = value_type &;
        using const_reference        = const value_type &;
        using pointer                = value_type *;
        using const_pointer          = const value_type *;
        using iterator               = value_type *;
        using const_iterator         = const value_type *;
        using reverse_iterator       = std::reverse_iterator<value_type *>;
        using const_reverse_iterator = std::reverse_iterator<const value_type *>;

        explicit constexpr vector() noexcept = default;

        template <class... Args, typename = std::enable_if_t<(std::is_constructible_v<value_type, Args &&> && ...)> >
        explicit constexpr vector(Args &&... args);

        constexpr vector(const vector &other);

        constexpr vector(vector &&other) noexcept;

        constexpr vector &operator=(const vector &other);

        constexpr vector &operator=(vector &&other) noexcept;

        ~vector();


        /*** Element access ***/

        constexpr reference operator[](size_type i);

        constexpr const_reference operator[](size_type i) const;

        [[nodiscard]] constexpr reference at(size_type pos);

        [[nodiscard]] constexpr const_reference at(size_type pos) const;

        [[nodiscard]] constexpr reference front();

        [[nodiscard]] constexpr const_reference front() const;

        [[nodiscard]] constexpr reference back();

        [[nodiscard]] constexpr const_reference back() const;

        [[nodiscard]] constexpr value_type *data() noexcept;

        [[nodiscard]] constexpr const value_type *data() const noexcept;


        /*** Iterators ***/

        [[nodiscard]] constexpr iterator begin() noexcept;

        [[nodiscard]] constexpr const_iterator begin() const noexcept;

        [[nodiscard]] constexpr const_iterator cbegin() const noexcept;

        [[nodiscard]] constexpr iterator end() noexcept;

        [[nodiscard]] constexpr const_iterator end() const noexcept;

        [[nodiscard]] constexpr const_iterator cend() const noexcept;

        [[nodiscard]] constexpr reverse_iterator rbegin() noexcept;

        [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept;

        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept;

        [[nodiscard]] constexpr reverse_iterator rend() noexcept;

        [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept;

        [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept;


        /*** Capacity ***/

        [[nodiscard]] constexpr bool empty() const noexcept;

        [[nodiscard]] constexpr size_type size() const noexcept;

        [[nodiscard]] constexpr size_type max_size() const noexcept;


        /*** Modifiers ***/
        constexpr void clear() noexcept;

        constexpr iterator insert(const_iterator pos, const value_type &value);

        constexpr iterator insert(const_iterator pos, value_type &&value);

        constexpr iterator insert(const_iterator pos, size_type count, const value_type &value);

        template <class InputIt, typename = std::enable_if_t<std::is_base_of_v<
                      std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category> > >
        constexpr iterator insert(const_iterator pos, InputIt first, InputIt last);

        constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> initializer_list);

        template <class... Args>
        constexpr iterator emplace(const_iterator pos, Args &&... args);

        constexpr iterator erase(const_iterator pos);

        constexpr iterator erase(const_iterator first, const_iterator last);

        constexpr reference push_back(const value_type &value);

        constexpr reference push_back(value_type &&value);

        template <class... Args>
        constexpr reference emplace_back(Args &&... args);

        constexpr void pop_back();

    private:
        alignas(value_type) std::byte storage_[sizeof(value_type) * max_elements]{};
        size_type                     size_{};
        size_type                     max_size_{ max_elements };

        [[nodiscard]] pointer ptr_at(size_type index) noexcept;

        [[nodiscard]] const_pointer ptr_at(size_type index) const noexcept;

        template <typename S, std::size_t SizeA, std::size_t SizeB>
        friend constexpr bool
        operator==(vector<S, SizeA> &,
                   vector<S, SizeB> &);
    };
}

template <typename T, std::size_t maximum_size>
cesa::vector<T, maximum_size>::~vector()
{
    clear();
}

template <typename T, std::size_t maximum_size>
template <class... Args, typename>
constexpr
cesa::vector<T, maximum_size>::vector(Args &&... args)
{
    static_assert(sizeof...(args) <= maximum_size, "Too many arguments");
    ((emplace_back(std::forward<Args>(args))), ...);
}

template <typename T, std::size_t maximum_size>
constexpr
cesa::vector<T, maximum_size>::vector(const vector &other)
{
    for (size_type i{}; i < other.size_; ++i)
    {
        emplace_back(*other.ptr_at(i));
    }
    size_ = other.size_;
}

template <typename T, std::size_t maximum_size>
constexpr
cesa::vector<T, maximum_size>::vector(vector &&other) noexcept
{
    for (size_type i{}; i < other.size_; ++i)
    {
        emplace_back(std::move(*other.ptr_at(i)));
    }
    size_ = other.size_;
    other.clear();
}

template <typename T, std::size_t maximum_size>
constexpr cesa::vector<T, maximum_size> &
cesa::vector<T, maximum_size>::operator=(const vector &other)
{
    if (this != &other)
    {
        clear();
        for (size_type i{}; i < other.size_; ++i)
        {
            emplace_back(*other.ptr_at(i));
        }
        size_ = other.size_;
    }
    return *this;
}

template <typename T, std::size_t maximum_size>
constexpr cesa::vector<T, maximum_size> &
cesa::vector<T, maximum_size>::operator=(vector &&other) noexcept
{
    if (this != &other)
    {
        clear();
        for (size_type i{}; i < other.size_; ++i)
        {
            emplace_back(std::move(*other.ptr_at(i)));
        }
        size_ = other.size_;
        other.clear();
    }
    return *this;
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::reference
cesa::vector<T, maximum_size>::operator[](const size_type i)
{
    return *ptr_at(i);
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_reference
cesa::vector<T, maximum_size>::operator[](const size_type i) const
{
    return *ptr_at(i);
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::reference
cesa::vector<T, maximum_size>::at(const size_type pos)
{
    if (pos >= size_)
    {
        throw std::out_of_range("index out of range");
    }
    return *ptr_at(pos);
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_reference
cesa::vector<T, maximum_size>::at(const size_type pos) const
{
    if (pos >= size_)
    {
        throw std::out_of_range("index out of range");
    }
    return *ptr_at(pos);
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::reference
cesa::vector<T, maximum_size>::front()
{
    return *ptr_at(0);
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_reference
cesa::vector<T, maximum_size>::front() const
{
    return *ptr_at(0);
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::reference
cesa::vector<T, maximum_size>::back()
{
    return *ptr_at(size_ - 1);
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_reference
cesa::vector<T, maximum_size>::back() const
{
    return *ptr_at(size_ - 1);
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::value_type *
cesa::vector<T, maximum_size>::data() noexcept
{
    return reinterpret_cast<value_type *>(storage_);
}

template <typename T, std::size_t maximum_size>
constexpr const typename cesa::vector<T, maximum_size>::value_type *
cesa::vector<T, maximum_size>::data() const noexcept
{
    return reinterpret_cast<const value_type *>(storage_);
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::iterator
cesa::vector<T, maximum_size>::begin() noexcept
{
    return data();
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_iterator
cesa::vector<T, maximum_size>::begin() const noexcept
{
    return data();
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_iterator
cesa::vector<T, maximum_size>::cbegin() const noexcept
{
    return data();
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::iterator
cesa::vector<T, maximum_size>::end() noexcept
{
    return data() + size_;
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_iterator
cesa::vector<T, maximum_size>::end() const noexcept
{
    return data() + size_;
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_iterator
cesa::vector<T, maximum_size>::cend() const noexcept
{
    return data() + size_;
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::reverse_iterator
cesa::vector<T, maximum_size>::rbegin() noexcept
{
    return reverse_iterator(end());
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_reverse_iterator
cesa::vector<T, maximum_size>::rbegin() const noexcept
{
    return const_reverse_iterator(end());
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_reverse_iterator
cesa::vector<T, maximum_size>::crbegin() const noexcept
{
    return const_reverse_iterator(end());
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::reverse_iterator
cesa::vector<T, maximum_size>::rend() noexcept
{
    return reverse_iterator(begin());
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_reverse_iterator
cesa::vector<T, maximum_size>::rend() const noexcept
{
    return const_reverse_iterator(begin());
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::const_reverse_iterator
cesa::vector<T, maximum_size>::crend() const noexcept
{
    return const_reverse_iterator(begin());
}

template <typename T, std::size_t maximum_size>
constexpr bool
cesa::vector<T, maximum_size>::empty() const noexcept
{
    return size_ == 0;
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::size_type
cesa::vector<T, maximum_size>::size() const noexcept
{
    return size_;
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::size_type
cesa::vector<T, maximum_size>::max_size() const noexcept
{
    return max_size_;
}

template <typename T, std::size_t maximum_size>
constexpr void
cesa::vector<T, maximum_size>::clear() noexcept
{
    if constexpr (std::is_trivially_destructible_v<value_type>)
    {
        size_ = 0;
    }
    else
    {
        while (size_ > 0)
        {
            pop_back();
        }
    }
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::iterator
cesa::vector<T, maximum_size>::insert(const_iterator pos, const value_type &value)
{
    return emplace(pos, value);
}

template <typename T, std::size_t max_elements>
constexpr typename cesa::vector<T, max_elements>::iterator
cesa::vector<T, max_elements>::insert(const_iterator pos, value_type &&value)
{
    return emplace(pos, std::move(value));
}

template <typename T, std::size_t max_elements>
constexpr typename cesa::vector<T, max_elements>::iterator
cesa::vector<T, max_elements>::insert(const_iterator pos, size_type count, const value_type &value)
{
    iterator it = begin() + std::distance(cbegin(), pos);
    while (count-- > 0)
    {
        it = insert(it, value);
        ++it;
    }
    return it;
}

template <typename T, std::size_t max_elements>
template <class InputIt, typename>
constexpr typename cesa::vector<T, max_elements>::iterator
cesa::vector<T, max_elements>::insert(const_iterator pos, InputIt first, InputIt last)
{
    iterator it = begin() + std::distance(cbegin(), pos);
    while (first != last)
    {
        it = insert(it, *first);
        ++first;
        ++it;
    }
    return it;
}

template <typename T, std::size_t max_elements>
constexpr typename cesa::vector<T, max_elements>::iterator
cesa::vector<T, max_elements>::insert(const const_iterator              pos,
                                                  std::initializer_list<value_type> initializer_list)
{
    return insert(pos, initializer_list.begin(), initializer_list.end());
}

template <typename T, std::size_t maximum_size>
template <class... Args>
constexpr typename cesa::vector<T, maximum_size>::iterator
cesa::vector<T, maximum_size>::emplace(const_iterator pos, Args &&... args)
{
    if (size_ >= maximum_size)
    {
        throw std::out_of_range("vector capacity exceeded");
    }
    const size_type index = pos == cend() ? size_ : std::distance(cbegin(), pos);
    if (index < size_)
    {
        std::move_backward(begin() + index, end(), end() + 1);
    }
    new(ptr_at(index)) value_type(std::forward<Args>(args)...);
    size_ += 1;
    return begin() + index;
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::iterator
cesa::vector<T, maximum_size>::erase(const_iterator pos)
{
    size_type index = std::distance(cbegin(), pos);
    if (index < size_)
    {
        std::move(begin() + index + 1, end(), begin() + index);
        pop_back();
    }
    return begin() + index;
}

template <typename T, std::size_t max_elements>
constexpr typename cesa::vector<T, max_elements>::iterator
cesa::vector<T, max_elements>::erase(const_iterator first, const_iterator last)
{
    size_type index = std::distance(cbegin(), first);
    size_type count = std::distance(first, last);
    if (index < size_)
    {
        std::move(begin() + index + count, end(), begin() + index);
        if constexpr (!std::is_default_constructible_v<value_type>)
        {
            for (auto it = begin() + index; it != end(); ++it)
            {
                it->~value_type();
            }
        }
    }
    size_ -= count;

    return begin() + index;
}

template <typename T, std::size_t max_elements>
constexpr typename cesa::vector<T, max_elements>::reference
cesa::vector<T, max_elements>::push_back(const value_type &value)
{
    return *emplace(end(), value);
}

template <typename T, std::size_t maximum_size>
constexpr typename cesa::vector<T, maximum_size>::reference
cesa::vector<T, maximum_size>::push_back(value_type &&value)
{
    return *emplace(end(), std::move(value));
}

template <typename T, std::size_t maximum_size>
template <class... Args>
constexpr typename cesa::vector<T, maximum_size>::reference
cesa::vector<T, maximum_size>::emplace_back(Args &&... args)
{
    return *emplace(end(), std::forward<Args>(args)...);
}

template <typename T, std::size_t maximum_size>
constexpr void
cesa::vector<T, maximum_size>::pop_back()
{
    if (size_ > 0)
    {
        size_ -= 1;
        if constexpr (!std::is_trivially_destructible_v<value_type>)
        {
            ptr_at(size_)->~value_type();
        }
    }
}

template <typename T, std::size_t max_elements>
typename cesa::vector<T, max_elements>::pointer
cesa::vector<T, max_elements>::ptr_at(const size_type index) noexcept
{
    return reinterpret_cast<pointer>(&storage_[index * sizeof(value_type)]);
}

template <typename T, std::size_t max_elements>
typename cesa::vector<T, max_elements>::const_pointer
cesa::vector<T, max_elements>::ptr_at(const size_type index) const noexcept
{
    return reinterpret_cast<const_pointer>(&storage_[index * sizeof(value_type)]);
}

template <typename S, std::size_t SizeA, std::size_t SizeB>
constexpr bool
operator==(cesa::vector<S, SizeA> &lhs,
           cesa::vector<S, SizeB> &rhs)
{
    return lhs.size_ == rhs.size_ && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

#endif
