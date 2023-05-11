#pragma once
#include <cstdint>
#include <iostream>
#include <source_location>

namespace tech {
template <class T> class List {
  public:
	/* member types */

	using value_type = T;
	struct Node {
		T value;
		Node* next;
		Node* prev;
		// Node(const T &val, Node *_prev = nullptr, Node *_next = nullptr)
		// 	: value(val), prev(_prev), next(_next) {}
		// Node(T &&val, Node *_prev = nullptr, Node *_next = nullptr)
		// 	: value(std::forward<T>(val)), prev(_prev), next(_next) {}
		template <class... Args>
		Node(Args&&... args)
			: value(std::forward<Args>(args)...), next(nullptr),
			  prev(nullptr) {}
		Node(const Node& node)
			: value(node.value), next(node.next), prev(node.next) {}
		// template <class... Args>
		// Node(const Args &...args, Node *_prev = nullptr, Node *_next =
		// nullptr) 	: value(args...), prev(_prev), next(_next) {}
	};
	using node_type = Node;
	template <class ValueType> class Iterator {
	  protected:
		Node* current;

	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = ValueType;
		using pointer = ValueType*;
		using reference = ValueType&;
		using iterator_category = std::bidirectional_iterator_tag;
		explicit Iterator(Node* ptr) : current(ptr) {}
		Iterator(const Iterator& it) = default;
		reference operator*() { return current->value; }
		pointer operator->() { return &(current->value); }
		bool operator==(const Iterator& another) const {
			return current == another.current;
		}
		Iterator& operator++() {
			if (current) {
				current = current->next;
			}
			return *this;
		}
		Iterator operator++(int) {
			auto old = *this;
			++(*this);
			return old;
		}
		Iterator& operator--() {
			if (current) {
				current = current->prev;
			}
			return *this;
		}
		Iterator operator--(int) {
			auto old = *this;
			--(*this);
			return old;
		}
	};
	using iterator = Iterator<T>;
	using const_iterator = Iterator<const T>;
	class NodeIterator {
	  protected:
		Node* current;

	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = Node*;
		using pointer = NodeIterator*;
		using reference = value_type&;
		using iterator_category = std::bidirectional_iterator_tag;
		explicit NodeIterator(Node* ptr) : current(ptr) {}
		NodeIterator(const NodeIterator& it) = default;
		reference operator*() { return current; }
		pointer operator->() { return this; }
		bool operator==(const NodeIterator& another) const {
			return current == another.current;
		}
		NodeIterator& operator++() {
			if (current) {
				current = current->next;
			}
			return *this;
		}
		NodeIterator operator++(int) {
			auto old = *this;
			++(*this);
			return old;
		}
		NodeIterator& operator--() {
			if (current) {
				current = current->prev;
			}
			return *this;
		}
		NodeIterator operator--(int) {
			auto old = *this;
			--(*this);
			return old;
		}
	};

	/* constructors*/
	List() : first(nullptr), last(nullptr) {
		first = nullptr;
		last = nullptr;
		count = 0;
	}
	// List(std::initializer_list<value_type> init) {}

	/* rule of 5 */
	List(const List& list) {
		for (const auto& item : list) {
			push_back(item);
		}
	}
	List(List&& list) noexcept
		: first(list.first), last(list.last), count(list.count) {
		list.first = nullptr;
		list.last = nullptr;
		list.count = 0;
	}
	List& operator=(const List& list) {
		clear();
		for (const auto item : list) {
			push_back(item);
		}
		return *this;
	}
	List& operator=(List&& list) {
		clear();
		first = list.first;
		last = list.last;
		count = list.count;
		list.first = nullptr;
		list.last = nullptr;
		list.count = 0;
		return *this;
	}
	~List() { clear(); }

	/* element access */
	T& front() { return (*first).value; };
	T& back() { return (*last).value; }

	/* iterators */
	Iterator<T> begin() noexcept { return Iterator<T>(first); }
	Iterator<T> end() noexcept { return Iterator<T>(nullptr); }
	Iterator<const T> begin() const noexcept {
		return Iterator<const T>(first);
	}
	Iterator<const T> end() const noexcept {
		return Iterator<const T>(nullptr);
	}
	Iterator<const T> cbegin() const noexcept {
		return Iterator<const T>(first);
	}
	Iterator<const T> cend() const noexcept {
		return Iterator<const T>(nullptr);
	}

	NodeIterator nbegin() noexcept { return NodeIterator(first); }
	NodeIterator nend() noexcept { return NodeIterator(nullptr); }

	/* capacity */
	std::size_t size() const noexcept { return count; }

	/* modifiers */
	void clear() noexcept {
		count = 0;
		auto current = first;
		while (current) {
			auto next = current->next;
			std::destroy_at(&(current->value));
			std::allocator<Node>().deallocate(current, 1);
			if (current == last) {
				break;
			}
			current = next;
		}
		first = nullptr;
		last = nullptr;
	}
	Node* push_back(const T& value) {
		Node* node = std::allocator<Node>().allocate(1);
		std::construct_at(&(node->value), value);
		return push_back(node);
	}
	Node* push_back(T&& value) {
		Node* node = std::allocator<Node>().allocate(1);
		std::construct_at(&(node->value), std::forward<T>(value));
		return push_back(node);
	}
	Node* push_back(Node* node) {
		if (first == nullptr) {
			first = node;
		}
		if (last) {
			last->next = node;
		}
		node->prev = last;
		node->next = nullptr;
		last = node;
		count++;
		return node;
	}
	template <class... Args> Node* emplace_back(Args&&... args) {
		Node* node = std::allocator<Node>().allocate(1);
		std::construct_at(&(node->value), std::forward<Args>(args)...);
		return push_back(node);
	}
	// template <class... Args> Node* emplace_back(const Args&... args) {
	// 	Node* node = new Node(args...);
	// 	return push_back(node);
	// }

	Iterator<T> erase(Iterator<T> pos) {}
	void pop_front() {
		auto next = first->next;
		std::destroy_at(&(first->value));
		std::allocator<Node>().deallocate(first, 1);
		first = next;
		count--;
	}

  private:
	Node* first = nullptr;
	Node* last = nullptr;
	std::size_t count = 0;
};
}