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
		Node(T val, Node *_prev = nullptr, Node* _next = nullptr) : value(val), prev(_prev), next(_next) {}
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

	/* constructors*/
	List() : first(nullptr), last(nullptr) {}

	/* rule of 5 */
	List(const List &list) {
		for (const auto& item : list) {
			push_back(item);
		}
	}
	List(List &&list)
		: first(list.first), last(list.last), count(list.count) {}
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
		return *this;
	}
	~List() { clear(); }

	/* element access */
	T &front() { return (*first).value; };
	T &back() { return (*last).value; }

	/* iterators */
	Iterator begin() { return Iterator(first); }
	Iterator end() { return Iterator(nullptr); }

	/* capacity */
	std::size_t size() const noexcept { return count; }

	/* modifiers */
	void clear() {
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
    Node* push_back(Node* node) {
		if (!first) {
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