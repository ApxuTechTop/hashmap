
#pragma once
#include <memory>
namespace tech {
template<class T, class Deleter = std::default_delete<T>>
    class UniquePtr {
        private:
        T* pointer;
        Deleter deleter;
        void clear() {
            if (pointer) {
                deleter(pointer);
            }
        }
        public:
        explicit UniquePtr(T* ptr = nullptr, Deleter d = Deleter()) noexcept : pointer(ptr), deleter(d) {
        }
        ~UniquePtr() noexcept {
            clear();
        }
        UniquePtr(UniquePtr&& uniqptr) noexcept : pointer(uniqptr.release()), deleter(std::move(uniqptr.deleter)) { // move constructor просто забрать владение
        }
        UniquePtr(const UniquePtr& ptr) = delete;
        template<class U, class UDeleter = Deleter> 
        UniquePtr(UniquePtr<U, UDeleter>&& ptr) noexcept : pointer(static_cast<T*>(ptr.release())), deleter(std::move(ptr.get_deleter())) {} // move constructor от другого класса
        


        UniquePtr& operator=(UniquePtr&& uniqptr) noexcept { // move assignment
            reset(uniqptr.release());
            deleter = std::move(uniqptr.get_deleter());
            return *this;
        }
        template<class U, class UDeleter = Deleter>
        UniquePtr& operator=(UniquePtr<U, UDeleter>&& uniqptr) noexcept {
            reset(static_cast<T*>(uniqptr.release()));
            deleter = std::move(uniqptr.get_deleter());
            return *this;
        }
        UniquePtr& operator=(const UniquePtr& ptr) = delete;
        T& operator*() const noexcept {
            return *pointer;
        }
        T* operator->() const noexcept {
            return pointer;
        }
        T* get() const noexcept {
            return pointer;
        }
        Deleter get_deleter() const noexcept {
            return deleter;
        }
        void reset(T* ptr = nullptr) noexcept {
            clear();
            pointer = ptr;
        }
        T* release() noexcept {
            T* tmpptr = pointer;
            pointer = nullptr;
            return tmpptr;
        }
    };
}