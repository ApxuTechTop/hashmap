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
	template <class ValueType> class Iterator {
	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = ValueType;
		using pointer = ValueType*;
		using reference = ValueType&;
		using iterator_category = std::bidirectional_iterator_tag;
		using hashmap_type = HashMap<Key, T, Hash>;

	  private:
		HashMap* map;
		value_type* current;
		//
		typename Vector<List<ValueType>>::template Iterator<List<ValueType>>
			bucket_iterator;
		//
		typename List<ValueType>::template Iterator<ValueType> list_iterator;

	  public:
		Iterator(HashMap* ptr)
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
		Iterator(const Iterator& it) = default;
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
	using iterator = Iterator<value_type>;
	using const_iterator = Iterator<const value_type>;

	/* constructors */
	HashMap() : buckets(1), hash({}), items_count(0) {
		buckets.reserve(1);
		buckets.resize(1);
	}
	// HashMap(std::initializer_list<value_type> init) {}
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
	Iterator<value_type> begin() noexcept {
		return Iterator<value_type>(this);
	}
	Iterator<value_type> end() noexcept {
		return Iterator<value_type>(nullptr);
	}
	Iterator<const value_type> begin() const noexcept {
		return Iterator<const value_type>(this);
	}
	Iterator<const value_type> end() const noexcept {
		return Iterator<const value_type>(nullptr);
	}
	Iterator<const value_type> cbegin() const noexcept {
		return Iterator<const value_type>(this);
	}
	Iterator<const value_type> cend() const noexcept {
		return Iterator<const value_type>(nullptr);
	}

	/* capacity */
	size_type size() const noexcept { return items_count; }
	bool empty() const noexcept { return size() == 0; }

	/* lookup */
	T& operator[](const Key& key) {
		std::size_t h = hash(key);
		std::size_t pos = bucket_index(h);
		auto bucket = buckets[pos];
		auto finded = find_value(bucket, key);
		if (finded) {
			return finded->second;
		}

		// finded = bucket.push_back({key, T()});
		auto& value = (bucket.emplace_back(key, T()))->value;
		items_count++;
		return value.second;

		// node_type* node = reinterpret_cast<node_type*>(operator
		// new(sizeof(node_type))); // выделили память new
		// (&(node->value.first)) Key(key); // сконструировали там ключ
		// finded = bucket.push_back(node); // засунули в список
	}
	T& operator[](Key&& key) {
		std::size_t h = hash(key);
		std::cout << "hash(key) == " << h << '\n';
		std::size_t pos = bucket_index(h);
		std::cout << "pos == " << pos << '\n';
		std::cout << "buckets.size() == " << buckets.size() << '\n';
		auto& bucket = buckets[pos];
		auto* finded = find_value(bucket, key);
		std::cerr << "finded == " << finded << '\n';
		if (finded) {
			return finded->second;
		}
		auto& value
			= (bucket.emplace_back(std::piecewise_construct,
								   std::forward_as_tuple(std::move(key)),
								   std::tuple<>()))
				  ->value;
		std::cout << "bucket.size() == " << bucket.size() << '\n';
		std::cout << "finded == " << finded << '\n';
		items_count++;
		return value.second;
	}
	T& at(const Key& key) {
		std::size_t h = hash(key);
		std::size_t pos = bucket_index(h);
		auto bucket = buckets[pos];
		auto finded = find_value(bucket, key);
		if (!finded) {
			throw std::out_of_range("No value with key\n");
		}
		return finded->second;
	}
	const T& at(const Key& key) const {
		std::size_t h = hash(key);
		std::size_t pos = bucket_index(h);
		auto bucket = buckets[pos];
		auto finded = find_value(bucket, key);
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
	}
	const_iterator find(const Key& key) const {
		for (auto it = cbegin(); it != cend(); ++it) { // итерируемся по мапе
			if (it->first == key) { // если нашли нужную пару
				return it;
			}
		}
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
		Vector<List<std::pair<Key, T>>> new_buckets;
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