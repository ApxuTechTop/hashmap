#pragma once

#include <cmath>
#include <functional>
#include <libtech/list.hpp>
#include <libtech/vector.hpp>
#include <utility>

#include <iostream>

namespace tech {
template <class Key, class T, class Hash = std::hash<Key>> class HashMap {
  public:
	using size_type = std::size_t;
	using value_type = std::pair<Key, T>;
	using key_type = Key;
	using mapped_type = T;
	using bucket_type = List<value_type>;
	using node_type = typename bucket_type::node_type;

  private:
	static constexpr const std::size_t INIT_BUCKET_COUNT = 1;
	static constexpr const float DEFAULT_MAX_LOAD_FACTOR = 1;
	Vector<bucket_type> buckets;
	Hash hash;
	size_type items_count;
	float max_saturation = DEFAULT_MAX_LOAD_FACTOR;

  public:
	template <class ValueType, class HashMapType> class Iterator {
	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = ValueType;
		using pointer = ValueType*;
		using reference = ValueType&;
		using iterator_category = std::bidirectional_iterator_tag;

	  private:
		HashMapType* map;
		ValueType* current;
		// typename Vector<List<ValueType>>::template Iterator<List<ValueType>>
		// 	bucket_iterator;
		// typename List<ValueType>::template Iterator<ValueType> list_iterator;
		decltype(map->buckets.end()) bucket_iterator;
		decltype(bucket_iterator->begin()) list_iterator;
	  public:
		Iterator(HashMapType* ptr)
			: map(ptr), current(nullptr), bucket_iterator(nullptr),
			  list_iterator(nullptr) {
			if (map == nullptr) {
				current = nullptr;
				return;
			}
			bucket_iterator = map->buckets.begin();
			if (bucket_iterator == map->buckets.end()) {
				current = nullptr;
				return;
			}
			list_iterator = bucket_iterator->begin();
			if (list_iterator == bucket_iterator->end()) {
				current = nullptr;
				return;
			}
			current = &(*list_iterator);
		}
		Iterator(HashMapType* ptr, bucket_type* bucket, node_type* node) : map(ptr), bucket_iterator(bucket), list_iterator(node) {
			current = &(*list_iterator);
		}
		reference operator*() { return *current; }
		pointer operator->() { return current; }
		bool operator==(const Iterator& another) const {
			return current == another.current;
		}
		Iterator& operator++() {
			if (current) { // если у нас до этого что то было
				if (++list_iterator == bucket_iterator->end()) {
					// если это последний в ведре, меняем ведро
					if (++bucket_iterator == map->buckets.end()) {
						// если это последнее ведро, то все
						current = nullptr;
						return *this;
					}
					list_iterator = bucket_iterator->begin();
				}
				current = &(*list_iterator);
			}
			return *this;
		}
		Iterator operator++(int) {
			auto old = *this;
			++(*this);
			return old;
		}
		Iterator& operator--() {
			if (current) { // если у нас до этого что то было
				if (list_iterator == bucket_iterator->begin()) {
					// если это первый в ведре, меняем ведро
					if (bucket_iterator == map->buckets.begin()) {
						// если это первое ведро, то все
						current = nullptr;
						return *this;
					}
					list_iterator = --(bucket_iterator->end());
				}
				current = &(*(--list_iterator));
			}
			return *this;
		} // не факт что это работает, надо протестировать
		Iterator operator--(int) {
			auto old = *this;
			--(*this);
			return old;
		}
	};
	using iterator = Iterator<value_type, HashMap>;
	using const_iterator = Iterator<const value_type, const HashMap>;

	/* constructors */
	HashMap() : buckets(1), hash({}), items_count(0) {
		buckets.reserve(1);
		buckets.resize(1);
	}
	template<class InputIt>
	HashMap(InputIt first, InputIt last, size_type buckets_count = 1, const Hash& _hash = Hash()) : buckets(buckets_count), hash(_hash), items_count(0) {
		buckets.resize(std::distance(first, last));
		insert(first, last);
	}
	HashMap(std::initializer_list<value_type> init, size_type buckets_count = 1, const Hash& _hash = Hash()) : HashMap(init.begin(), init.end(), buckets_count, _hash) {}
	explicit HashMap(size_type buckets_count, const Hash& _hash = Hash())
		: buckets(buckets_count), hash(_hash), items_count(0) {
		buckets.resize(buckets_count);
	}

	/* rule of 5 */
	HashMap(const HashMap& other)
		: buckets(other.buckets), hash(other.hash_function()),
		  items_count(other.size()) {}
	HashMap(HashMap&& other) noexcept
		: buckets(std::move(other.buckets)),
		  hash(std::move(other.hash_function())), items_count(other.size()) {
		other.items_count = 0;
	}
	HashMap& operator=(const HashMap& other) {
		buckets = other.buckets;
		hash = other.hash_function();
		items_count = other.size();
	}
	HashMap& operator=(HashMap&& other) {
		buckets = std::move(other.buckets);
		hash = std::move(other.hash_function());
		items_count = other.size();
		other.items_count = 0;
	}

	/* iterators */
	iterator begin() noexcept {
		return iterator(this);
	}
	iterator end() noexcept {
		return iterator(static_cast<HashMap*>(nullptr));
	}
	const_iterator begin() const noexcept {
		return const_iterator(this);
	}
	const_iterator end() const noexcept {
		return const_iterator(static_cast<const HashMap*>(nullptr));
	}
	const_iterator cbegin() const noexcept {
		return const_iterator(this);
	}
	const_iterator cend() const noexcept {
		return const_iterator(static_cast<const HashMap*>(nullptr));
	}

	/* capacity */
	size_type size() const noexcept { return items_count; }
	bool empty() const noexcept { return size() == 0; }

	/* modifiers */
	void clear() noexcept {
		buckets.clear();
		items_count = 0;
	}
	std::pair<iterator, bool> insert(const value_type& value) {
		return emplace(value);
	}
	std::pair<iterator, bool> insert(value_type&& value) {
		return emplace(std::forward<value_type>(value));
	}
	template<class InputIt>
	void insert(InputIt first, InputIt last) {
		for (auto it = first; it != last; ++it) {
			insert(*it);
		}
	}

	template<class... Args>
	std::pair<iterator, bool> emplace(Args&&... args) {
		// создать элемент, проверить есть ли с таким ключом, если есть уничтожить созданный, если нет увеличить? вектор, вставить элемент
		auto* node = new typename tech::List<value_type>::node_type(std::forward<Args>(args)...);
		auto finded = find(node->value.first);
		if (finded != end()) {
			delete node;
			return {finded, false};
		}
		if ((size() + 1) > (max_load_factor() * bucket_count())) {
			rehash(bucket_count() + 1); // проработать стратегию расширения
		}
		std::size_t h = hash(node->value.first);
		std::size_t pos = bucket_index(h);
		auto& bucket = buckets[pos];
		bucket.push_back(node);
		iterator it(this, &bucket, node);
		return {it, true};
	}
	template<class... Args>
	std::pair<iterator, bool> try_emplace(const Key& key, Args&&... args) {
		return emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(std::forward<Args>(args)...));
	}
	template<class... Args>
	std::pair<iterator, bool> try_emplace(Key&& key, Args&&... args) {
		return emplace(std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple(std::forward<Args>(args)...));
	}


	/* lookup */
	T& operator[](const Key& key) {
		return try_emplace(key).first->second;

		// node_type* node = reinterpret_cast<node_type*>(operator
		// new(sizeof(node_type))); // выделили память new
		// (&(node->value.first)) Key(key); // сконструировали там ключ
		// finded = bucket.push_back(node); // засунули в список
	}
	T& operator[](Key&& key) {
		return try_emplace(std::move(key)).first->second; // в cppreference используется std::move а не std::forward
		// std::size_t h = hash(key);
		// std::size_t pos = bucket_index(h);
		// auto& bucket = buckets[pos];
		// auto* finded = find_value(bucket, key);
		// if (finded) {
		// 	return finded->second;
		// }
		// auto& value
		// 	= (bucket.emplace_back(std::piecewise_construct,
		// 						   std::forward_as_tuple(std::move(key)),
		// 						   std::tuple<>()))
		// 		  ->value;
		// items_count++;
		// return value.second;
	}
	T& at(const Key& key) {
		std::size_t h = hash(key);
		std::size_t pos = bucket_index(h);
		auto& bucket = buckets[pos];
		auto* finded = find_value(bucket, key);
		if (!finded) {
			throw std::out_of_range("No value with key\n");
		}
		return finded->second;
	}
	const T& at(const Key& key) const {
		std::size_t h = hash(key);
		std::size_t pos = bucket_index(h);
		auto& bucket = buckets[pos];
		auto* finded = find_value(bucket, key);
		if (!finded) {
			throw std::out_of_range("No value with key\n");
		}
		return finded->second;
	}
	size_type count(const Key& key) const {
		return buckets[bucket_index(hash(key))].size();
	}
	iterator find(const Key& key) {
		for (auto it = begin(); it != end(); ++it) { // итерируемся по мапе
			if (it->first == key) { // если нашли нужную пару
				return it;
			}
		}
		return end();
	}
	const_iterator find(const Key& key) const {
		for (auto it = begin(); it != end(); ++it) { // итерируемся по мапе
			if (it->first == key) { // если нашли нужную пару
				return it;
			}
		}
		return end();
	}
	bool contains(const Key& key) const {
		return find(key) != end();
	}

	/* bucket interface */
	size_type bucket_count() const { return buckets.size(); }
	size_type bucket_size(size_type n) const { return buckets[n].size(); }

	/* hash policy */
	float load_factor() const { return size() / bucket_count(); }
	void rehash(size_type count) {
		// надо изменить количество ведер и перенести
		// указатели на Node без создания новых объектов,
		// просто работа с указателями
		if (count == buckets.size()) {
			return;
		}
		auto old_buckets = std::move(buckets);
		Vector<bucket_type> new_buckets;
		if (count * max_load_factor() < size()) {
			count = size() / max_load_factor();
		}
		if (count == new_buckets.capacity()) {
			return;
		}
		new_buckets.reserve(count);
		new_buckets.resize(count);

		node_type* node = nullptr;
		// нужно проитерировать по нодам
		for (auto bucket_it = old_buckets.begin();
			 bucket_it != old_buckets.end(); ++bucket_it) {
			for (auto list_it = bucket_it->nbegin();
				 list_it != bucket_it->nend();
				 ++list_it) { // nbegin - итератор по нодам operator* -> Node*
							  // operator-> -> this
				if (node != nullptr) {
					auto h = hash(node->value.first);
					new_buckets[h % new_buckets.capacity()].push_back(node);
				}
				node = *list_it;

				// возможна проблема из-за push_back указатели будут меняться а
				// значит и итератор может сломаться, решение: сделать задержку
			}
		}
		auto h = hash(node->value.first);
		new_buckets[h % new_buckets.capacity()].push_back(
			node); // не забыть про последнюю ноду из за задержки
		buckets = std::move(new_buckets);
	}
	void reserve(size_type count) {
		rehash(std::ceil(count / max_load_factor()));
	}
	void max_load_factor(float lf) {
		max_saturation = lf;
		if (load_factor() > max_saturation) {
			reserve(size());
		}
	} // если lf < 0 то что?
	float max_load_factor() const { return max_saturation; }
	value_type* find_value(bucket_type& bucket, const Key& key) {
		// std::cout << "find_node(bucket, " << key << ")\n";
		for (auto& pair : bucket) {
			// std::cout << "pair.first == " << pair.first << '\n';
			if (pair.first == key) {
				return &pair;
			}
		}
		return nullptr;
	}
	std::size_t bucket_index(std::size_t h) { return h % buckets.capacity(); }
};
}