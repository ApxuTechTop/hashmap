#pragma once

#include <functional>
#include <unordered_map>

namespace tech {
    template<class Key, class T, class Hash = std::hash<Key>, class Allocator = std::allocator<std::pair<Key, T>>>
    class HashMap {
        private:
            static size_type init_bucket_count = 2003;
            tech::Vector<tech::List<std::pair<Key, T>>> buckets;
            Hash hash;
            Allocator allocator;
            size_type items_count;
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
            using bucket_type = tech::List<value_type>
            using node_type = bucket_type::node_type


            /* constructors */
            HashMap() : HashMap() {}
            HashMap(std::initializer_list<value_type> init) {}
            explicit HashMap(size_type bucket_count, const Hash& _hash = Hash(), const Allocator& alloc = Allocator()) : hash(_hash), allocator(alloc) {}
            ~HashMap() {}

            /* iterators */
            iterator begin() noexcept {}
            iterator end() noexcept {}
            //const_iterator cbegin() const noexcept {}
            //const_iterator cend() const noexcept {}

            /* capacity */
            bool empty() const {}
            size_type size() const {return items_count;}

            /* lookup */
            T& operator[](const Key& key) {
                std::size_t h = hash(key);
                std::size_t pos = bucket_index(h);
                auto bucket = buckets[pos];
                auto finded = find_node(bucket, key);
                if (!finded) {
                    node_type* node = operator new(sizeof(node_type)); // выделили память
                    new (&(node->value.first)) Key(key); // сконструировали там ключ
                    finded = bucket.push_back(node); // засунули в список
                    items_count++;
                }
                return finded->value.second; // вернули ссылку
            }
            T& operator[](Key&& key) {}
            T& at(const Key& key) {}
            const T& at(const Key& key) const {}
            size_type count(const key& key) const {return buckets[bucket_index(hash(key))].size();}
            iterator find(const Key& key) {}
            //const_iterator find(const key& key) const {}


            /* hash policy */
            float load_factor() const {}
            void rehash(size_type count) {}
            void reserve(size_type count) {}
            void max_load_factor(float lf) {}
            float max_load_factor() const {}

    };
}