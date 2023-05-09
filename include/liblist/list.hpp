#pragma once

namespace tech {
template <class T> class List {
	public:
	/* member types */
    using iterator = Iterator;
    using node_type = Node;
	struct Node {
		T value;
		Node *next;
        Node *prev;
		Node(const T& val, Node *_prev = nullptr, Node* _next = nullptr) : value(val), prev(_prev), next(_next) {}
        Node(T&& val, Node *_prev = nullptr, Node* _next = nullptr) : value(std::forward<T>(val)), prev(_prev), next(_next) {}
		template<class ...Args>
		Node(Args&&... args, Node *_prev = nullptr, Node* _next = nullptr) : value(std::forward<Args>(args)), prev(_prev), next(_next) {}
		template<class ...Args>
		Node(const Args&... args, Node *_prev = nullptr, Node* _next = nullptr) : value(args), prev(_prev), next(_next) {}
	};
	class Iterator {
	  protected:
		Node *current;

	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T *;
		using reference = T &;
		using iterator_category = std::bidirectional_iterator_tag;
		explicit Iterator(Node *ptr) : current(ptr) {}
		Iterator(const Iterator &it) = default;
		reference operator*() { return current->value; }
		pointer operator->() { return &(current->value); }
		bool operator==(const Iterator &another) const {
			return current == another.current;
		}
		Iterator &operator++() {
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
        Iterator &operator--() {
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
	class NodeIterator {
	  protected:
		Node *current;

	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = Node*;
		using pointer = NodeIterator *;
		using reference = value_type &;
		using iterator_category = std::bidirectional_iterator_tag;
		explicit Iterator(Node *ptr) : current(ptr) {}
		Iterator(const Iterator &it) = default;
		reference operator*() { return current; }
		pointer operator->() { return this; }
		bool operator==(const Iterator &another) const {
			return current == another.current;
		}
		Iterator &operator++() {
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
        Iterator &operator--() {
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

	/* constructors*/
	List() : first(nullptr), last(nullptr) {}
	List(std::initializer_list<value_type> init) {}

	/* rule of 5 */
	List(const List &list) {
		for (const auto& item : list) {
			push_back(item);
		}
	}
	List(List &&list)
		: first(list.first), last(list.last), count(list.count) {
			list.first = nullptr;
			list.last = nullptr;
			list.count = 0;
	}
	List &operator=(const List &list) {
		clear();
		for (const auto item : list) {
			push_back(item);
		}
		return *this;
	}
	List &operator=(List &&list) {
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
	T &front() { return (*first).value; };
	T &back() { return (*last).value; }

	/* iterators */
	constexpr Iterator begin() noexcept { return Iterator(first); }
	constexpr Iterator end() noexcept { return Iterator(nullptr); }

	constexpr NodeIterator nbegin() noexcept {return NodeIterator(first);}
	constexpr NodeIterator nend() noexcept {return NodeIterator(nullptr);}

	/* capacity */
	std::size_t size() const noexcept { return count; }

	/* modifiers */
	void clear() noexcept {
		count = 0;
		auto current = first;
		while (current) {
			auto next = current->next;
			delete current;
			if (current == last) {
				break;
			}
			current = next;
		}
		first = nullptr;
		last = nullptr;
	}
	Node *push_back(const T &value) {
		Node *node = new Node(value, nullptr, nullptr);
        return push_back(node);
	}
	Node *push_back(T &&value) {
		Node *node = new Node(std::forward<T>(value), nullptr, nullptr);
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
		last = node;
		count++;
		return node;
    }
	template<class ...Args>
	Node* emplace_back(Args&&... args) {
		Node* node = new Node(std::forward<Args>(args), nullptr, nullptr);
		return push_back(node);
	}
	template<class ...Args>
	Node* emplace_back(const Args&... args) {
		Node* node = new Node(args, nullptr, nullptr);
		return push_back(node);
	}

    iterator erase(iterator pos) {

    }
	void pop_front() {
		auto next = first->next;
		delete first;
		first = next;
		count--;
	}
	

  private:
	Node *first;
	Node *last;
	std::size_t count = 0;
};
}