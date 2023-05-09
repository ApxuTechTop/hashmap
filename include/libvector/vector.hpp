#include <memory>
#include <cstdint>

namespace tech {
    template<class T, class Allocator = std::allocator<T>> 
    class Vector {
        private:
        T* _items;
        size_type _count;
        size_type _capacity;
        constexpr void clean() {
            clear();
            Allocator().deallocate(_items, capacity());
        }
        public:
        /* member types */
        using size_type = std::size_t
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = T*;
        class Iterator {

        };

        /* constructors */
        constexpr Vector() noexcept(noexcept(Allocator())) : _items(nullptr), _count(0), _capacity(0) {}
        constexpr Vector(size_type count) : _items(Allocator().allocate(count)), _count(0), _capacity(count) {}
        constexpr Vector(std::initializer_list<T> init) {}

        /* rule of 5 */
        constexpr Vector(const Vector& other) noexcept : _items(Allocator().allocate(other.capacity())), _count(other.size()), _capacity(other.capacity()) {
            std::uninitialized_copy_n(other.data(), other.size(), _items);
        }
        constexpr Vector(Vector&& other) noexcept : _items(other.data()), _count(other.size()), _capacity(other.capacity()) {
            other._items = nullptr;
            other._count = 0;
            other._capacity = 0;
        }
        constexpr Vector& operator=(const Vector& other) {
            clean();
            _items = Allocator().allocate(other.capacity());
            _capacity = other.capacity();
            _count = other.size();
            std::uninitialized_copy_n(other.data(), other.size(), _items);
        }
        constexpr Vector& operator=(Vector&& other) {
            clean();
            _items = other.data();
            _count = other.size();
            _capacity = other.capacity();
            other._items = nullptr;
            other._count = 0;
            other._capacity = 0;
        }
        constexpr ~Vector() {clean();}


        /* element access */
        constexpr reference at(size_type pos) {
            if (!(pos < size())) {
                throw std::out_of_range("Out of range\n");
            }
            return _items[pos];
        }
        constexpr reference operator[](size_type pos) {
            return _items[pos];
        }
        constexpr reference front() {
            return _items[0];
        }
        constexpr reference back() {
            return _items[size()];
        }
        constexpr pointer data() {
            return _items;
        }

        /* capacity */
        [[nodiscard]] constexpr bool empty() const noexcept {return _count == 0;}
        constexpr size_type size() const noexcept {return _count;}
        constexpr void reserve(size_type new_cap) {
            if (new_cap < capacity()) {
                return;
            }
            // TODO
        } // TODO
        constexpr size_type capacity() const noexcept {return _capacity;}
        constexpr void shrink_to_fit() {}

        /* modifiers */
        constexpr void clear() noexcept {
            std::destroy_n(data(), size());
            _count = 0;
        }
        constexpr void push_back(const T& value) {
            if (_count == _capacity) {
                reserve(_capacity * 2);
            }
            _items[_count++] = value; // ?
        }
        constexpr void push_back(T&& value) {
            if (_count == _capacity) {
                reserve(_capacity * 2);
            }
            _items[_count++] = value; // ?
        }
        constexpr void pop_back() {}
        constexpr void resize(size_type _count, const T& value) {}


    };
}