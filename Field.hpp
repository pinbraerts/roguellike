#ifndef ROGUELLIKE_FIELD_HPP
#define ROGUELLIKE_FIELD_HPP

#include <memory>

template<class T> class index_iterator {
protected:
    T& t;
    size_t i;

public:
    index_iterator(T& _t, size_t _i = 0) noexcept: t(_t), i(_i) {}
    index_iterator(index_iterator&& other) noexcept: t(other.t), i(other.i) {}
    index_iterator(const index_iterator& other) noexcept: t(other.t), i(other.i) {}

    auto& operator*() {
        return t[i];
    }
    index_iterator& operator++() {
        ++i;
        return *this;
    }
    index_iterator& operator--() {
        --i;
        return *this;
    }
    index_iterator& operator+=(size_t n) {
        i += n;
        return *this;
    }
    index_iterator& operator-=(size_t n) {
        i -= n;
        return *this;
    }
    bool operator<(const index_iterator& other) const {
        return i < other.i;
    }
    bool operator>(const index_iterator& other) const {
        return i > other.i;
    }
    bool operator!=(const index_iterator& other) const {
        return i != other.i;
    }
};
template<class T> class const_index_iterator {
protected:
    const T& t;
    size_t i;

public:
    explicit const_index_iterator(T& _t, size_t _i = 0) noexcept: t(_t), i(_i) {}
    const_index_iterator(const_index_iterator&& other) noexcept: t(other.t), i(other.i) {}
    const_index_iterator(const const_index_iterator& other) noexcept: t(other.t), i(other.i) {}
    const_index_iterator(index_iterator<T>&& other) noexcept: t(other.t), i(other.i) {}
    const_index_iterator(const index_iterator<T>& other) noexcept: t(other.t), i(other.i) {}

    const auto& operator*() {
        return t[i];
    }
    const_index_iterator& operator++() {
        ++i;
        return *this;
    }
    const_index_iterator& operator--() {
        --i;
        return *this;
    }
    const_index_iterator& operator+=(size_t n) {
        i += n;
        return *this;
    }
    const_index_iterator& operator-=(size_t n) {
        i -= n;
        return *this;
    }
    bool operator<(const const_index_iterator& other) const {
        return i < other.i;
    }
    bool operator>(const const_index_iterator& other) const {
        return i > other.i;
    }
    bool operator!=(const const_index_iterator& other) const {
        return i != other.i;
    }
};

template<class T> class Field {
private:
    T* d;
    std::size_t w, h;

    void alloc(std::size_t _w, std::size_t _h) {
        clear();
        if(_w != 0 && _h != 0)
            d = new T[_w * _h];
    }
    void alloc() {
        alloc(w, h);
    }

public:
    explicit Field(std::size_t _w = 0, std::size_t _h = 0): d(nullptr), w(_w), h(_h) {
        alloc();
    }
    Field(const Field& f): w(f.w), h(f.h) {
        alloc();
    }
    Field(Field&& f) noexcept: d(f.d), w(f.w), h(f.h) {
        f.d = nullptr;
        f.w = f.h = 0;
    }

    void resize(std::size_t _w, std::size_t _h) {
        if(_w == w && _h == h)
            return;
        alloc(_w, _h);
        w = _w;
        h = _h;
    }
    void clear() {
        if(d != nullptr) {
            for (T* x = d, * e = d + w * h; x < e; ++x)
                x->~T();
            delete[] d;
            d = nullptr;
        }
    }
    void swap(Field& other) {
        Field f(std::move(other));
        new (&other) Field(std::move(*this));
        new (this) Field(std::move(f));
    }

    bool empty() const {
        return w == 0 || h == 0;
    }
    explicit operator bool() {
        return !empty();
    }
    std::size_t width() const {
        return w;
    }
    std::size_t height() const {
        return h;
    }
    std::size_t size() const {
        return w * h;
    }
    T* data() {
        return d;
    }
    const T* data() const {
        return d;
    }

    T& at(std::size_t x, std::size_t y) {
        return d[y * w + x];
    }
    const T& at(std::size_t x, std::size_t y) const {
        return d[y * w + x];
    }

    struct field_column: index_iterator<Field> {
        using base = index_iterator<Field>;
        using base::base;
        using base::t;
        using base::i;

        field_column& operator*() {
            return *this;
        }

        T& operator[](std::size_t y) {
            return t.at(i, y);
        }
        explicit operator bool() {
            return t;
        }


        using iterator = index_iterator<field_column>;

        iterator begin() {
            return { *this };
        }
        iterator end() {
            return { *this, t.height() };
        }
    };
    Field::field_column operator[](std::size_t x) {
        return { *this, x };
    }
    field_column column_begin() {
        return { *this };
    }
    field_column column_end() {
        return { *this, w };
    }
    class column_iterable {
    private:
        Field& f;

    public:
        column_iterable(Field& _f): f(_f) {}

        field_column begin() {
            return f.column_begin();
        }
        field_column end() {
            return f.column_end();
        }
    };
    column_iterable columns() {
        return { *this };
    }

    struct field_row: index_iterator<Field> {
        using base = index_iterator<Field>;
        using base::base;
        using base::t;
        using base::i;

        field_row& operator*() {
            return *this;
        }

        T& operator[](std::size_t x) {
            return t.at(x, i);
        }
        explicit operator bool() {
            return t;
        }

        T* begin() {
            return t.data() + i * t.width();
        }
        T* end() {
            return t.data() + (i + 1) * t.width();
        }
    };
    Field::field_row begin() {
        return { *this };
    }
    Field::field_row end() {
        return { *this, h };
    }
    class row_iterable {
        Field& f;

    public:
        row_iterable(Field& _f): f(_f) {}

        field_row begin() {
            return f.begin();
        }
        field_row end() {
            return f.end();
        }
    };
    row_iterable rows() {
        return { *this };
    }

    class raw_iterable {
        Field& m;

    public:
        raw_iterable(Field& _m): m(_m) {}

        T* begin() {
            return m.data();
        }
        T* end() {
            return m.data() + m.size();
        }
    };
    raw_iterable raw() {
        return { *this };
    }

    // TODO: add const iterators

    ~Field() {
        clear();
    }
};

#endif // !ROGUELLIKE_FIELD_HPP
