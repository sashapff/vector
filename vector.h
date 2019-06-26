//
// Created by Alexandra Ivanova on 21/06/2019.
//

#ifndef VECTOR_VECTOR_H
#define VECTOR_VECTOR_H

#include <variant>

template<typename T>
struct iterator {
    typedef T value_type;
    typedef T &reference;
    typedef std::ptrdiff_t difference_type;
    typedef T *pointer;
    typedef std::random_access_iterator_tag iterator_category;

    template<typename> friend
    class vector;

    template<typename> friend
    class vector_const_iterator;

    iterator() = default;

    iterator(iterator const &) = default;

    iterator &operator=(iterator const &) = default;

    iterator &operator++() {
        ++ptr;
        return *this;
    }

    const iterator operator++(int) {
        iterator result(*this);
        ++*this;
        return result;
    }

    iterator &operator--() {
        --ptr;
        return *this;
    }

    const iterator operator--(int) {
        iterator result(*this);
        --*this;
        return result;
    }

    reference operator*() const {
        return *ptr;
    }

    pointer operator->() const {
        return ptr;
    }

    bool operator==(iterator const &other) const {
        return ptr == other.ptr;
    }

    bool operator!=(iterator const &other) const {
        return ptr != other.ptr;
    }

    bool operator<(iterator const &other) {
        return ptr < other.ptr;
    }

    bool operator>(iterator const &other) {
        return ptr > other.ptr;
    }

    bool operator<=(iterator const &other) {
        return ptr <= other.ptr;
    }

    bool operator>=(iterator const &other) {
        return ptr >= other.ptr;
    }

    iterator &operator+=(size_t n) {
        ptr += n;
        return *this;
    }

    iterator &operator-=(size_t n) {
        ptr -= n;
        return *this;
    }

    reference operator[](size_t n) {
        return ptr[n];
    }

    friend difference_type operator-(iterator const &p, iterator const &q) {
        return p.ptr - q.ptr;
    }

    friend iterator operator+(iterator p, size_t n) {
        p += n;
        return p;
    }

    friend iterator operator-(iterator p, size_t n) {
        p -= n;
        return p;
    }

    friend iterator operator+(size_t n, iterator const &p) {
        p += n;
        return p;
    }

    pointer ptr = nullptr;

private:
    explicit iterator(pointer p) : ptr(p) {}
};

template<typename T>
struct const_iterator {
    typedef T value_type;
    typedef T const &reference;
    typedef std::ptrdiff_t difference_type;
    typedef T *pointer;
    typedef std::random_access_iterator_tag iterator_category;

    template<typename> friend
    class vector;

    const_iterator() = default;

    const_iterator(const_iterator const &) = default;

    const_iterator(iterator<T> const &other) : ptr(other.ptr) {}

    const_iterator &operator=(const_iterator const &) = default;

    const_iterator &operator++() {
        ++ptr;
        return *this;
    }

    const const_iterator operator++(int) {
        const_iterator result(*this);
        ++*this;
        return result;
    }

    const_iterator &operator--() {
        --ptr;
        return *this;
    }

    const const_iterator operator--(int) {
        const_iterator result(*this);
        --*this;
        return result;
    }

    reference operator*() const {
        return *ptr;
    }

    pointer operator->() const {
        return ptr;
    }

    bool operator==(const_iterator const &other) const {
        return ptr == other.ptr;
    }

    bool operator!=(const_iterator const &other) const {
        return ptr != other.ptr;
    }

    bool operator<(const_iterator const &other) {
        return ptr < other.ptr;
    }

    bool operator>(const_iterator const &other) {
        return ptr > other.ptr;
    }

    bool operator<=(const_iterator const &other) {
        return ptr <= other.ptr;
    }

    bool operator>=(const_iterator const &other) {
        return ptr >= other.ptr;
    }

    const_iterator &operator+=(size_t n) {
        ptr += n;
        return *this;
    }

    const_iterator &operator-=(size_t n) {
        ptr -= n;
        return *this;
    }

    reference operator[](size_t n) {
        return ptr[n];
    }

    friend difference_type operator-(const_iterator const &p,
                                     const_iterator const &q) {
        return p.ptr - q.ptr;
    }

    friend const_iterator operator+(const_iterator p, size_t n) {
        p += n;
        return p;
    }

    friend const_iterator operator-(const_iterator p, size_t n) {
        p -= n;
        return p;
    }

    friend const_iterator operator+(size_t n, const_iterator const &p) {
        p += n;
        return p;
    }

private:
    explicit const_iterator(pointer p) : ptr(p) {}

    pointer ptr = nullptr;
};

template<typename T>
class vector {
public:
    typedef T value_type;
    typedef T *pointer;
    typedef T const *const_pointer;
    typedef T &reference;
    typedef T const &const_reference;

    typedef iterator<T> iterator;
    typedef const_iterator<T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    vector() noexcept = default;

    ~vector() {
        if (!is_ptr_type()) {
            variant = nullptr;
        } else {
            if (std::get<0>(variant) != nullptr) {
                free_check(std::get<0>(variant));
            }
        }
    }

    vector(vector const &other) {
        if (is_ptr_type()) {
            free_check(std::get<0>(variant));
        }
        variant = other.variant;
        if (is_ptr_type() && std::get<0>(variant)) {
            counter_in_ptr(std::get<0>(variant))++;
        }
    }

    template<typename InputIterator>
    vector(InputIterator first, InputIterator last) {
        if (first + 1 == last) {
            variant = *first;
        } else {
            auto ptr = allocate(last - first);
            std::uninitialized_copy(first, last, reinterpret_cast<pointer>(ptr));
            set_size(ptr, last - first);
            set_capacity(ptr, last - first);
            set_counter(ptr, 1);
            variant = ptr;
        }
    }

    vector &operator=(vector const &other) {
        if (this == &other) {
            return *this;
        }
        if (is_ptr_type()) {
            free_check(std::get<0>(variant));
        }
        variant = other.variant;
        if (is_ptr_type() && std::get<0>(variant)) {
            counter_in_ptr(std::get<0>(variant))++;
        }
        return *this;
    }


    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        vector(first, last).swap(*this);
        return *this;
    }

    reference operator[](size_t i) {
        if (is_ptr_type()) {
            copy_if_necessary(std::get<0>(variant));
            auto ptr = std::get<0>(variant);
            return get_data(std::get<0>(variant))[i];
        }
        return std::get<1>(variant);
    }

    const_reference operator[](size_t i) const noexcept {
        if (is_ptr_type()) {
            return get_data_const(std::get<0>(variant))[i];
        }
        return std::get<1>(variant);
    }

    reference front() {
        if (is_ptr_type()) {
            return get_data(std::get<0>(variant))[0];
        }
        return std::get<1>(variant);
    }

    const_reference front() const {
        if (is_ptr_type()) {
            return get_data_const(std::get<0>(variant))[0];
        }
        return std::get<1>(variant);
    }

    reference back() {
        if (is_ptr_type()) {
            return get_data(std::get<0>(variant))[size_in_ptr(std::get<0>(variant)) - 1];
        }
        return std::get<1>(variant);
    }

    const_reference back() const {
        if (is_ptr_type()) {
            return get_data_const(std::get<0>(variant))[size_in_ptr(std::get<0>(variant)) - 1];
        }
        return std::get<1>(variant);
    }

    void push_back(const_reference a) {
        if (empty()) {
            if (capacity() == 0) {
                try {
                    variant = a;
                } catch (...) {
                    variant = nullptr;
                    throw;
                }
                return;
            } else {
                only_push_back(a);
            }
        } else if (size() == capacity()) {
            allocate_and_push_back(a);
        } else {
            only_push_back(a);
        }
        size_in_ptr(std::get<0>(variant))++;
    };

    void pop_back() {
        if (!is_ptr_type() && size() == 1) {
            variant = nullptr;
        } else {
            std::destroy(get_data(std::get<0>(variant)) + size_in_ptr(std::get<0>(variant)) - 1,
                         get_data(std::get<0>(variant)) + size_in_ptr(std::get<0>(variant)));
            size_in_ptr(std::get<0>(variant))--;
        }
    }

    pointer data() {
        if (is_ptr_type()) {
            return get_data(std::get<0>(variant));
        }
        return &std::get<1>(variant);
    }

    const_pointer data() const {
        if (is_ptr_type()) {
            return get_data_const(std::get<0>(variant));
        }
        return &std::get<1>(variant);
    }

    iterator begin() {
        return iterator(data());
    }

    const_iterator begin() const noexcept {
        return const_iterator(get_data());
    }

    iterator end() {
        return iterator(data() + size());
    }

    const_iterator end() const noexcept {
        return const_iterator(get_data() + size());
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    bool empty() const noexcept {
        return is_ptr_type() &&
               (!std::get<0>(variant) || (std::get<0>(variant) && size_in_ptr(std::get<0>(variant)) == 0));
    }

    size_t size() const noexcept {
        return is_ptr_type() ? (empty() ? 0 : size_in_ptr(std::get<0>(variant))) : 1;
    }

    void convert() {
        auto new_ptr = reinterpret_cast<info_pointer>(operator new(3 * sizeof(size_t) + sizeof(value_type)));
        size_in_ptr(new_ptr) = 1;
        capacity_in_ptr(new_ptr) = 1;
        counter_in_ptr(new_ptr) = 1;
        try {
            construct(get_data(new_ptr), std::get<1>(variant));
        } catch (...) {
            free_empty(new_ptr);
            throw;
        }
        variant = new_ptr;
    }

    void reserve(size_t cap) {
        if (cap > capacity()) {
            if (!is_ptr_type()) {
                convert();
            }
            auto ptr = allocate_and_copy(cap, std::get<0>(variant));
            free_check(std::get<0>(variant));
            variant = ptr;
        }
    }

    size_t capacity() const noexcept {
        return is_ptr_type() ? (std::get<0>(variant) ? capacity_in_ptr(std::get<0>(variant)) : 0) : 1;
    }

//            shrink_to_fit
//    resize
//            clear

    iterator insert(const_iterator pos, T const &val) {
        if (pos == end()) {
            push_back(val);
            return end() - 1;
        }
        if (is_ptr_type()) {
            info_pointer new_ptr = allocate((size() == capacity()) ? capacity() * 2 : capacity());
            pointer ptr = get_data(new_ptr);
            try {
                std::uninitialized_copy(begin(), iterator(pos.ptr), ptr);
            } catch (...) {
                free_empty(new_ptr);
                throw;
            }
            size_in_ptr(new_ptr) = pos - begin();
            capacity_in_ptr(new_ptr) = size() == capacity() ? capacity() * 2 : capacity();
            counter_in_ptr(new_ptr) = 1;
            ptr += pos - begin();
            try {
                construct(ptr, val);
            } catch (...) {
                free_always(new_ptr);
                throw;
            }
            ++size_in_ptr(new_ptr);
            ++ptr;
            try {
                std::uninitialized_copy(iterator(pos.ptr), end(), iterator(ptr));
            } catch (...) {
                free_always(new_ptr);
                throw;
            }
            size_in_ptr(new_ptr) += end() - pos;
            free_check(std::get<0>(variant));
            variant = new_ptr;
            return iterator(get_data(new_ptr) + (pos - begin()));
        }
        if (pos != begin()) {
            throw std::runtime_error("incorrect position");
        }
        vector<value_type> buffer;
        buffer.push_back(*begin());
        buffer.push_back(val);
        swap(buffer);
        return begin();
    }

    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }

    iterator erase(const_iterator first, const_iterator last) {
        if (first == last) {
            return iterator(first.ptr);
        }
        if (is_ptr_type()) {
            copy_if_necessary(std::get<0>(variant));
            if (last == end()) {
                std::destroy(first.ptr, last.ptr);
                size_t delta = last - first;
                size_in_ptr(std::get<0>(variant)) -= delta;
                return end();
            }
            size_t begin_size = first - begin();
            size_t erase_size = last - first;
            size_t end_size = end() - last;
            pointer begin_ptr = data();
            pointer erase_ptr = data() + begin_size;
            pointer end_ptr = data() + begin_size + erase_size;
            if (end_size <= erase_size) {
                std::destroy(erase_ptr, erase_ptr + erase_size);
                try {
                    std::uninitialized_copy(end_ptr, end_ptr + end_size, erase_ptr);
                } catch (...) {
                    std::destroy(end_ptr, end_ptr + end_size);
                    size_in_ptr(std::get<0>(variant)) = begin_size;
                    throw;
                }
                std::destroy(end_ptr, end_ptr + end_size);
            } else {
                std::destroy(erase_ptr, erase_ptr + erase_size);
                try {
                    std::uninitialized_copy(end_ptr, end_ptr + erase_size, erase_ptr);
                } catch (...) {
                    std::destroy(end_ptr, end_ptr + end_size);
                    size_in_ptr(std::get<0>(variant)) = begin_size;
                    throw;
                }
                try {
                    for (pointer to = end_ptr, from = end_ptr + erase_size; from < end_ptr + end_size; ++from, ++to) {
                        *to = *from;
                    }
                } catch (...) {
                    std::destroy(erase_ptr, end_ptr + end_size);
                    size_in_ptr(std::get<0>(variant)) = begin_size;
                    throw;
                }
                std::destroy(end_ptr + end_size - erase_size, end_ptr + end_size);
            }
            size_in_ptr(std::get<0>(variant)) = begin_size + end_size;
            return begin() + begin_size;
        }
        pop_back();
        return begin();
    }

    void swap(vector &other) {
        variant.swap(other.variant);
    }

private:
    typedef char *info_pointer;
    std::variant<info_pointer, value_type> variant;

    pointer data(info_pointer ptr) const noexcept {
        return reinterpret_cast<pointer>(ptr + 3 * sizeof(size_t));
    }

    pointer get_data() const noexcept {
        if (!is_ptr_type()) {
            return empty() ? nullptr : const_cast<pointer>(&std::get<1>(variant));
        } else {
            return data(std::get<0>(variant));
        }
    }

    bool is_ptr_type() const noexcept {
        return variant.index() == 0;
    }

    size_t size_in_ptr(info_pointer ptr) const noexcept {
        return *reinterpret_cast<size_t *>(ptr);
    }

    size_t &size_in_ptr(info_pointer ptr) noexcept {
        return *reinterpret_cast<size_t *>(ptr);
    }

    size_t capacity_in_ptr(info_pointer ptr) const noexcept {
        return *reinterpret_cast<size_t *>(ptr + sizeof(size_t));
    }

    size_t &capacity_in_ptr(info_pointer ptr) noexcept {
        return *reinterpret_cast<size_t *>(ptr + sizeof(size_t));
    }

    size_t counter_in_ptr(info_pointer ptr) const noexcept {
        return *reinterpret_cast<size_t *>(ptr + 2 * sizeof(size_t));
    }

    size_t &counter_in_ptr(info_pointer ptr) noexcept {
        return *reinterpret_cast<size_t *>(ptr + 2 * sizeof(size_t));
    }

    pointer get_data(info_pointer ptr) noexcept {
        return reinterpret_cast<pointer>(ptr + 3 * sizeof(size_t));
    }

    const_pointer get_data_const(info_pointer const ptr) const noexcept {
        return reinterpret_cast<pointer>(ptr + 3 * sizeof(size_t));
    }

    void allocate_and_push_back(const_reference a) {
        if (!is_ptr_type()) {
            info_pointer ptr = nullptr;
            try {
                ptr = allocate(2);
                set_size(ptr, 1);
                set_capacity(ptr, 2);
                set_counter(ptr, 1);
                construct(get_data(ptr), std::get<1>(variant));
            } catch (...) {
                free_empty(ptr);
                throw;
            }
            try {
                construct(get_data(ptr) + 1, a);
            } catch (...) {
                free_always(ptr);
                throw;
            }
            variant = ptr;
            return;
        }
        info_pointer ptr = nullptr;
        ptr = allocate_and_copy(capacity() * 2, std::get<0>(variant));
        try {
            construct(get_data(ptr) + size_in_ptr(ptr), a);
        } catch (...) {
            free_always(ptr);
            throw;
        }
        free_check(std::get<0>(variant));
        variant = ptr;
    }

    void free_check(info_pointer ptr) {
        if (ptr != nullptr && --counter_in_ptr(ptr) == 0) {
            std::destroy(get_data(ptr), get_data(ptr) + size_in_ptr(ptr));
            free_empty(ptr);
        }
    }

    void free_always(info_pointer ptr) {
        std::destroy(get_data(ptr), get_data(ptr) + size_in_ptr(ptr));
        free_empty(ptr);
    }

    void free_empty(info_pointer ptr) {
        operator delete(static_cast<void *>(ptr));
    }

    void set_size(const info_pointer ptr, size_t sz) {
        size_in_ptr(ptr) = sz;
    }

    void set_capacity(const info_pointer ptr, size_t cap) {
        capacity_in_ptr(ptr) = cap;
    }

    void set_counter(const info_pointer ptr, size_t cnt) {
        counter_in_ptr(ptr) = cnt;
    }

    void construct(pointer ptr, const_reference a) {
        new(ptr) value_type(a);
    }

    void only_push_back(const_reference a) {
        try {
            construct(get_data(std::get<0>(variant)) + size_in_ptr(std::get<0>(variant)), a);
        } catch (...) {
            if (counter_in_ptr(std::get<0>(variant)) != 1) {
                free_always(std::get<0>(variant));
            }
            throw;
        }
    }

    info_pointer allocate(size_t sz) {
        return reinterpret_cast<info_pointer>(operator new(3 * sizeof(size_t) + sz * sizeof(value_type)));
    }

    info_pointer allocate_and_copy(size_t sz, info_pointer ptr) {
        auto new_ptr = reinterpret_cast<info_pointer>(operator new(3 * sizeof(size_t) + sz * sizeof(value_type)));
        size_in_ptr(new_ptr) = 0;
        capacity_in_ptr(new_ptr) = sz;
        counter_in_ptr(new_ptr) = 1;
        if (ptr == nullptr) {
            return new_ptr;
        }
        assert(sz >= size_in_ptr(ptr));
        try {
            size_in_ptr(new_ptr) = size_in_ptr(ptr);
            capacity_in_ptr(new_ptr) = sz;
            counter_in_ptr(new_ptr) = counter_in_ptr(ptr);
            std::uninitialized_copy(get_data(ptr), get_data(ptr) + size_in_ptr(ptr), get_data(new_ptr));
        } catch (...) {
            free_empty(new_ptr);
            throw;
        }
        return new_ptr;
    }

    void copy_if_necessary(info_pointer ptr) {
        if (counter_in_ptr(ptr) > 1) {
            auto new_ptr = reinterpret_cast<info_pointer>(operator new(
                    3 * sizeof(size_t) + size_in_ptr(ptr) * sizeof(value_type)));
            size_in_ptr(new_ptr) = size_in_ptr(ptr);
            capacity_in_ptr(new_ptr) = capacity_in_ptr(ptr);
            counter_in_ptr(new_ptr) = 1;
            counter_in_ptr(ptr)--;
            try {
                std::uninitialized_copy(get_data(ptr), get_data(ptr) + size_in_ptr(ptr), get_data(new_ptr));
            } catch (...) {
                free_empty(new_ptr);
                throw;
            }
            variant = new_ptr;
        }
    }

};


template<typename T>
void swap(vector<T> &a, vector<T> &b) {
    a.swap(b);
}

template<typename T>
bool operator==(vector<T> const &a, vector<T> const &b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

template<typename T>
bool operator!=(vector<T> const &a, vector<T> const &b) {
    return !(a == b);
}

template<typename T>
bool operator<(vector<T> const &a, vector<T> const &b) {
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template<typename T>
bool operator<=(vector<T> const &a, vector<T> const &b) {
    return a < b || a == b;
}

template<typename T>
bool operator>(vector<T> const &a, vector<T> const &b) {
    return b < a;
}

template<typename T>
bool operator>=(vector<T> const &a, vector<T> const &b) {
    return b <= a;
}

#endif //VECTOR_VECTOR_H
