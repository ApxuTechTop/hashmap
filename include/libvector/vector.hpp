#include <memory>

namespace tech {
    template<class T, class Allocator = std::allocator<T>> 
    class Vector {
        private:
        T* vector;
        size_type count;
        size_type max_count;
        constexpr void clean() {
            clear();

            Allocator().deallocate(vector, capacity());
        }
        static size_type init_capacity = 10;
        public:
        /* member types */
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = T*;
        class Iterator {

        };

        /* constructors */
        
        constexpr Vector() noexcept(noexcept(Allocator())) : vector(nullptr) {
            vector = Allocator().allocate(init_capacity);
            max_count = init_capacity
        }
        constexpr Vector(std::initializer_list<T> init) {}

        /* rule of 5 */
        constexpr Vector(const Vector& other) noexcept {}
        constexpr Vector(Vector&& other) noexcept {}
        constexpr Vector& operator=(const Vector& other) {}
        constexpr Vector& operator=(Vector&& other) {}
        constexpr ~Vector() {clean();}


        /* element access */
        constexpr reference at(size_type pos) {
            if (!(pos < size())) {
                throw std::out_of_range("Out of range\n");
            }
            return vector[pos];
        }
        constexpr reference operator[](size_type pos) {
            return vector[pos];
        }
        constexpr reference front() {
            return vector[0];
        }
        constexpr reference back() {
            return vector[size()];
        }
        constexpr pointer data() {
            return vector;
        }

        /* capacity */
        [[nodiscard]] constexpr bool empty() const noexcept {return count != 0;}
        constexpr size_type size() const noexcept {return count;}
        constexpr void reserve(size_type new_cap) {}
        constexpr size_type capacity() const noexcept {return max_count;}
        constexpr void shrink_to_fit() {}

        /* modifiers */
        constexpr void clear() noexcept {
            std::destroy_n(vector, size());
            count = 0;
        }
        constexpr void push_back(const T& value) {}
        constexpr void push_back(T&& value) {}
        constexpr void pop_back() {}
        constexpr void resize(size_type count, const T& value) {}


    };
}