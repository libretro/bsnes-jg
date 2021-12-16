#pragma once

namespace nall {

template<typename T> struct shared_pointer;

struct shared_pointer_manager {
  void* pointer = nullptr;
  function<void (void*)> deleter;
  uint strong = 0;
  uint weak = 0;

  shared_pointer_manager(void* pointer) : pointer(pointer) {
  }
};

template<typename T> struct shared_pointer;

template<typename T>
struct shared_pointer {
  template<typename... P> static auto create(P&&... p) {
    return shared_pointer<T>{new T{std::forward<P>(p)...}};
  }

  using type = T;
  shared_pointer_manager* manager = nullptr;

  template<typename U>
  struct is_compatible {
    static constexpr bool value = std::is_base_of<T, U>::value || std::is_base_of<U, T>::value;
  };

  shared_pointer() {
  }

  shared_pointer(T* source) {
    operator=(source);
  }

  template<typename U, typename = std::enable_if_t<is_compatible<U>::value>>
  shared_pointer(const shared_pointer<U>& source) {
    operator=<U>(source);
  }

  ~shared_pointer() {
    reset();
  }

  auto operator=(const shared_pointer& source) -> shared_pointer& {
    if(this != &source) {
      reset();
      if((bool)source) {
        manager = source.manager;
        manager->strong++;
      }
    }
    return *this;
  }

  template<typename U, typename = std::enable_if_t<is_compatible<U>::value>>
  auto operator=(shared_pointer&& source) -> shared_pointer& {
    if((uintptr_t)this != (uintptr_t)&source) {
      reset();
      manager = source.manager;
      source.manager = nullptr;
    }
    return *this;
  }

  auto data() -> T* {
    if(manager) return (T*)manager->pointer;
    return nullptr;
  }

  auto data() const -> const T* {
    if(manager) return (T*)manager->pointer;
    return nullptr;
  }

  auto operator->() -> T* { return data(); }
  auto operator->() const -> const T* { return data(); }

  auto operator*() -> T& { return *data(); }
  auto operator*() const -> const T& { return *data(); }

  auto operator()() -> T& { return *data(); }
  auto operator()() const -> const T& { return *data(); }

  template<typename U>
  auto operator==(const shared_pointer<U>& source) const -> bool {
    return manager == source.manager;
  }

  explicit operator bool() const {
    return manager && manager->strong;
  }

  auto unique() const -> bool {
    return manager && manager->strong == 1;
  }

  auto references() const -> uint {
    return manager ? manager->strong : 0;
  }

  auto reset() -> void {
    if(manager && manager->strong) {
      //pointer may contain weak references; if strong==0 it may destroy manager
      //as such, we must destroy strong before decrementing it to zero
      if(manager->strong == 1) {
        if(manager->deleter) {
          manager->deleter(manager->pointer);
        } else {
          delete (T*)manager->pointer;
        }
        manager->pointer = nullptr;
      }
      if(--manager->strong == 0) {
        if(manager->weak == 0) {
          delete manager;
        }
      }
    }
    manager = nullptr;
  }
};

}
