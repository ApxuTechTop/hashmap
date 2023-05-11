#include <cstdint>
#include <memory>

namespace tech {
template <class T, class Allocator = std::allocator<T>> class Vector {

  public:
	/* member types */
	using difference_type = std::ptrdiff_t;
	using size_type = std::size_t;
	using value_type = T;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = T*;

  private:
	T* _items;
	size_type _count;
	size_type _capacity;
	constexpr void clean() {
		std::destroy_n(_items, _count);
		_count = 0;
		Allocator().deallocate(_items, _capacity);
	}

  public:
	/* constructors */
	constexpr Vector() noexcept(noexcept(Allocator()))
		: _items(nullptr), _count(0), _capacity(0) {}
	constexpr Vector(size_type count)
		: _items(Allocator().allocate(count)), _count(0), _capacity(count) {}
	constexpr Vector(std::initializer_list<T> init) {}

	/* rule of 5 */
	constexpr Vector(const Vector& other) noexcept
		: _items(Allocator().allocate(other.capacity())), _count(other.size()),
		  _capacity(other.capacity()) {
		std::uninitialized_copy_n(other.data(), other.size(), _items);
	}
	constexpr Vector(Vector&& other) noexcept
		: _items(other.data()), _count(other.size()),
		  _capacity(other.capacity()) {
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
		return *this;
	}
	constexpr ~Vector() { clean(); }

	/* element access */
	constexpr reference at(size_type pos) {
		if (!(pos < _count)) {
			throw std::out_of_range("Out of range\n");
		}
		return _items[pos];
	}
	constexpr reference operator[](size_type pos) { return _items[pos]; }
	constexpr reference front() { return _items[0]; }
	constexpr reference back() { return _items[_count - 1]; }
	constexpr pointer data() { return _items; }

	template <class ValueType> class Iterator {
	  private:
		ValueType* current;

	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = ValueType;
		using pointer = ValueType*;
		using reference = ValueType&;
		using iterator_category = std::contiguous_iterator_tag;
		Iterator(value_type* ptr) : current(ptr) {}
		Iterator(const Iterator& it) = default;
		reference operator*() { return *current; }
		pointer operator->() { return current; }
		Iterator& operator++() {
			current += 1;
			return *this;
		}
		Iterator operator++(int) {
			auto old = *this;
			++(*this);
			return old;
		}
		Iterator& operator--() {
			current -= 1;
			return *this;
		}
		Iterator operator--(int) {
			auto old = *this;
			--(*this);
			return old;
		}
		reference operator[](const difference_type n) const noexcept {
			return *(current + n);
		}
		Iterator& operator+=(const difference_type n) noexcept {
			current += n;
			return *this;
		}
		Iterator& operator-=(const difference_type n) noexcept {
			current -= n;
			return *this;
		}
		difference_type operator-(const Iterator& it) const noexcept {
			return current - it.current;
		}
		Iterator operator-(const difference_type n) const noexcept {
			auto old = *this;
			old -= n;
			return old;
		}
		bool operator==(const Iterator& it) const noexcept {
			return current == it.current;
		}
		std::strong_ordering operator<=>(const Iterator& it) const noexcept {
			return current <=> it.current;
		}
	};

	/* iterators */
	Iterator<T> begin() { return Iterator<T>(_items); }
	Iterator<T> end() { return Iterator<T>(_items + size()); }
	Iterator<const T> begin() const { return Iterator<const T>(_items); }
	Iterator<const T> end() const {
		return Iterator<const T>(_items + size());
	}

	/* capacity */
	[[nodiscard]] constexpr bool empty() const noexcept { return _count == 0; }
	constexpr size_type size() const noexcept { return _count; }
	constexpr void reserve(size_type new_cap) {
		if (new_cap <= capacity()) {
			return;
		}
		value_type* new_items = Allocator().allocate(new_cap);
		if (_items != nullptr) { // если у нас что то было
			std::uninitialized_move_n(_items, size(), new_items);
			auto old_count = _count;
			clean();
			_count = old_count;
		}
		_capacity = new_cap;
		_items = new_items;
		// TODO
	} // TODO
	constexpr size_type capacity() const noexcept { return _capacity; }
	constexpr void shrink_to_fit() {}

	/* modifiers */
	constexpr void clear() noexcept {
		std::destroy_n(data(), size());
		_count = 0;
	}
	constexpr void push_back(const T& value) { emplace_back(value); }
	constexpr void push_back(T&& value) {
		emplace_back(std::forward<T>(value));
	}
	template <class... Args> constexpr reference emplace_back(Args&&... args) {
		if (_capacity == 0) {
			reserve(1);
		}
		if (_count == _capacity) {
			auto new_capacity = _capacity * 2;
			auto new_count = _count + 1;
			value_type* new_items = Allocator().allocate(_capacity * 2);
			std::construct_at(new_items + _count, std::forward<Args>(args)...);
			for (size_type i = 0; i < _count; ++i) {
				std::uninitialized_move_n(_items + i, 1, new_items + i);
				std::destroy_at(_items + i);
			}
			Allocator().deallocate(_items, _capacity);
			// std::uninitialized_move_n(_items, size(), new_items); надо
			// поэлементно перемещать clean();
			_items = new_items;
			_capacity = new_capacity;
			_count = new_count;
			return back();
		}
		std::construct_at(_items + _count++, std::forward<Args>(args)...);
		return back();
	}
	constexpr void pop_back() {
		std::destroy_at(_items + size() - 1);
		_count--;
		//_items[--_count].~T();
	}
	constexpr void resize(size_type new_count) {
		if (new_count == size()) {
			return;
		}
		if (new_count < size()) {
			std::destroy(Iterator<value_type>(_items + new_count), end());
		} else {
			if (new_count > capacity()) {
				reserve(new_count);
			}
			std::uninitialized_default_construct_n(end(), new_count - size());
			_count = new_count;
		}
	}
	// constexpr void resize(size_type new_count, const T& value) {
	// 	if (new_count == size()) {
	// 		return;
	// 	}
	// 	if (new_count < size()) {
	// 		std::destroy(Iterator<const value_type>(_items + new_count),
	// end()); 	} else { 		if (new_count > capacity()) {
	// reserve(new_count);
	// 		}
	// 		for (size_type i = 0; i < new_count; ++i) {
	// 			push_back(value);
	// 		}
	// 	}
	// }
};
}

template <class T, class ValueType>
typename tech::Vector<T>::template Iterator<ValueType>
operator+(typename tech::Vector<T>::template Iterator<ValueType>& it,
		  const typename tech::Vector<T>::template Iterator<
			  ValueType>::difference_type n) {
	return tech::Vector<T>::template Iterator<ValueType>(it.current + n);
}

// template <class T, class ValueType>
// 	tech::Vector<T>::template Iterator<ValueType> operator+(const typename
// tech::Vector<T>::template Iterator<ValueType>::difference_type n,
// tech::Vector<T>::template Iterator<ValueType> &it) { 		return typename
// tech::Vector<T>::template Iterator<ValueType>(it.current + n);
// 	}
// }