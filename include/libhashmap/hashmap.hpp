#pragma once

#include <functional>
#include <unordered_map>

namespace tech {
    template<class Key, class T, class Hash = std::hash<Key>, class Allocator = std::allocator<std::pair<Key, T>>>
    class HashMap {
        private:
            static const size_type INIT_BUCKET_COUNT = 1;
            static const float DEFAULT_MAX_LOAD_FACTOR = 1;
            tech::Vector<tech::List<std::pair<Key, T>>> buckets;
            Hash hash;
            Allocator allocator;
            size_type items_count;
            float max_saturation = DEFAULT_MAX_LOAD_FACTOR;
            static node_type* find_node(const bucket_type& bucket, const Key& key) {
                for (bucket_type::iterator it = bucket.begin(); it != bucket.end(); ++it) {
                    if (it.current->first == key) {
                        return it.current;
                    }
                }
                return nullptr;
            }
            std::size_t bucket_index(std::size_t h) {
                return h % buckets.capacity();
            }
        public:
            

            using size_type = std::size_t;
            using value_type = std::pair<Key, T>;
            using key_type = Key;
            using mapped_type = T;
            using bucket_type = tech::List<value_type>;
            using node_type = bucket_type::node_type;
            class Iterator {
                public:
                using difference_type = std::ptrdiff_t;
                using value_type = value_type;
                using pointer = value_type *;
                using reference = value_type &;
                using iterator_category = std::bidirectional_iterator_tag;
                explicit Iterator(HashMap* ptr) : bucket_iterator(map->buckets.begin()), list_iterator(bucket_iterator->begin()) {
                    map = ptr;
                    if (map == nullptr) {
                        current = nullptr;
                        return;
                    }
                    if (bucket_iterator == map->buckets.end()) {
                        current = nullptr;
                        return;
                    }
                    if (list_iterator == bucket_iterator->end()) {
                        current = nullptr;
                        return;
                    }
                    current = &(*list_iterator);
                }
                Iterator(const Iterator &it) = default;
                reference operator*() { return *current; }
                pointer operator->() { return current; }
                bool operator==(const Iterator &another) const {
                    return current == another.current;
                }
                Iterator &operator++() {
                    if (current) { // если у нас до этого что то было
                        if (++list_iterator == bucket_iterator->end()) { // если это последний в ведре, меняем ведро
                            if (++bucket_iterator == map->bucket.end()) { // если это последнее ведро, то все
                                current = nullptr;
                                return;
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
                Iterator &operator--() { 
                    if (current) { // если у нас до этого что то было
                        if (list_iterator == bucket_iterator->begin()) { // если это первый в ведре, меняем ведро
                            if (bucket_iterator == map->bucket.begin()) { // если это первое ведро, то все
                                current = nullptr;
                                return;
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
                private:
                HashMap* map = nullptr;
                value_type* current = nullptr;
                typename tech::Vector<bucket_type>::iterator bucket_iterator;
                typename bucket_type::iterator list_iterator;
            };
            class ConstIterator {
                public:
                using difference_type = std::ptrdiff_t;
                using value_type = const value_type;
                using pointer = value_type *;
                using reference = value_type &;
                using iterator_category = std::bidirectional_iterator_tag;
                explicit Iterator(HashMap* ptr) : bucket_iterator(map->buckets.begin()), list_iterator(bucket_iterator->begin()) {
                    map = ptr;
                    if (map == nullptr) {
                        current = nullptr;
                        return;
                    }
                    if (bucket_iterator == map->buckets.end()) {
                        current = nullptr;
                        return;
                    }
                    if (list_iterator == bucket_iterator->end()) {
                        current = nullptr;
                        return;
                    }
                    current = &(*list_iterator);
                }
                Iterator(const Iterator &it) = default;
                reference operator*() { return *current; }
                pointer operator->() { return current; }
                bool operator==(const Iterator &another) const {
                    return current == another.current;
                }
                Iterator &operator++() {
                    if (current) { // если у нас до этого что то было
                        if (++list_iterator == bucket_iterator->end()) { // если это последний в ведре, меняем ведро
                            if (++bucket_iterator == map->bucket.end()) { // если это последнее ведро, то все
                                current = nullptr;
                                return;
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
                Iterator &operator--() { 
                    if (current) { // если у нас до этого что то было
                        if (list_iterator == bucket_iterator->begin()) { // если это первый в ведре, меняем ведро
                            if (bucket_iterator == map->bucket.begin()) { // если это первое ведро, то все
                                current = nullptr;
                                return;
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
                private:
                HashMap* map = nullptr;
                value_type* current = nullptr;
                typename tech::Vector<bucket_type>::iterator bucket_iterator;
                typename bucket_type::iterator list_iterator;
            };
            using iterator = Iterator;
            using const_iterator = ConstIterator;

            /* constructors */
            HashMap() {
                buckets.reserve(1);
            }
            HashMap(std::initializer_list<value_type> init) {}
            explicit HashMap(size_type bucket_count, const Hash& _hash = Hash(), const Allocator& alloc = Allocator()) : buckets(bucket_count), hash(_hash), allocator(alloc) {}
            

            /* rule of 5 */
            HashMap(const HashMap& other) : buckets(other.buckets), hash(other.hash_function()), allocator(other.get_allocator()), items_count(other.size()) {}
            HashMap(HashMap&& other) : buckets(std::move(other.buckets)), hash(std::move(other.hash_function())), allocator(std::move(other.get_allocator())), items_count(other.size()) {
                other.items_count = 0;
            }
            HashMap& operator=(const HashMap& other) {}
            HashMap& operator=(HashMap&& other) {
                buckets = std::move(other.buckets);
                hash = std::move(other.hash_function());
                allocator = std::move(other.get_allocator());
                items_count = other.size();
                other.items_count = 0;
            }
            ~HashMap() {
                clean();
            }

            /* iterators */
            iterator begin() noexcept {return Iterator(this);}
            iterator end() noexcept {return Iterator(nullptr);}
            const_iterator cbegin() const noexcept {return ConstIterator(this);}
            const_iterator cend() const noexcept {return ConstIterator(nullptr);}

            /* capacity */
            bool empty() const noexcept {return size() == 0;}
            size_type size() const noexcept {return items_count;}

            /* lookup */
            T& operator[](const Key& key) {
                std::size_t h = hash(key);
                std::size_t pos = bucket_index(h);
                auto bucket = buckets[pos];
                auto finded = find_node(bucket, key);
                if (!finded) {
                    // finded = bucket.push_back({key, T()});
                    finded = bucket.emplace_back(key, T());
                    items_count++;

                    // node_type* node = reinterpret_cast<node_type*>(operator new(sizeof(node_type))); // выделили память
                    // new (&(node->value.first)) Key(key); // сконструировали там ключ
                    // finded = bucket.push_back(node); // засунули в список
                }
                return finded->value.second; // вернули ссылку
            }
            T& operator[](Key&& key) {
                std::size_t h = hash(key);
                std::size_t pos = bucket_index(h);
                auto bucket = buckets[pos];
                auto finded = find_node(bucket, key);
                if (!finded) {
                    // finded = bucket.push_back({std::forward<Key>(key), T()});
                    finded = bucket.emplace_back(std::forward<Key>(key), T());
                    items_count++;

                    // node_type* node = operator new(sizeof(node_type)); // выделили память
                    // new (&(node->value.first)) Key(std::forward(key)); // forward?
                    // finded = bucket.push_back(node); // засунули в список
                }
                return finded->value.second; // вернули ссылку
            }
            T& at(const Key& key) {
                std::size_t h = hash(key);
                std::size_t pos = bucket_index(h);
                auto bucket = buckets[pos];
                auto finded = find_node(bucket, key);
                if (!finded) {
                    throw std::out_of_range("No value with key\n");
                }
                return finded->value.second;
            }
            const T& at(const Key& key) const {
                std::size_t h = hash(key);
                std::size_t pos = bucket_index(h);
                auto bucket = buckets[pos];
                auto finded = find_node(bucket, key);
                if (!finded) {
                    throw std::out_of_range("No value with key\n");
                }
                return finded->value.second;
            }
            size_type count(const key& key) const {return buckets[bucket_index(hash(key))].size();}
            iterator find(const Key& key) {
                for (auto it = begin(); it != end(); ++it) { // итерируемся по мапе
                    if (it->first == key) { // если нашли нужную пару
                        return it;
                    }
                }
            }
            const_iterator find(const key& key) const {
                for (auto it = cbegin(); it != cend(); ++it) { // итерируемся по мапе
                    if (it->first == key) { // если нашли нужную пару
                        return it;
                    }
                }
            }

            /* bucket interface */
            size_type bucket_count() const {return buckets.size();}
            size_type bucket_size(size_type n) const {return buckets[n].size();}

            /* hash policy */
            float load_factor() const {return size() / bucket_count();}
            void rehash(size_type count) { // надо изменить количество ведер и перенести указатели на Node без создания новых объектов, просто работа с указателями
                auto old_buckets = std::move(buckets);
                tech::Vector<tech::List<std::pair<Key, T>>> new_buckets;
                if (count * max_load_factor() < size()) {
                    count = size() / max_load_factor();
                }
                if (count == new_buckets.capacity) {
                    return;
                }
                new_buckets.reserve(count);
                
                node_type* node = nullptr;
                // нужно проитерировать по нодам
                for (auto bucket_it = old_buckets.begin(); bucket_it != old_buckets.end(); ++bucket_it) {
                    for (auto list_it = bucket_it->nbegin(); list_it != bucket_it->nend(); ++list_it) { // nbegin - итератор по нодам operator* -> Node* operator-> -> this
                        if (node != nullptr) {
                            new_buckets[new_buckets.bucket_index(node->value.fisrt)].push_back(node);
                        }
                        node = *list_it;
                       
                        // возможна проблема из-за push_back указатели будут меняться а значит и итератор может сломаться, решение: сделать задержку
                    }
                }
                new_buckets[new_buckets.bucket_index(node->value.fisrt)].push_back(node); // не забыть про последнюю ноду из за задержки
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
            float max_load_factor() const {return max_saturation;}

    };
}