/* List of changes as per Zlib license terms:
   - Filenames modified to match bsnes convention
*/
#ifndef LIBBYUUML_QUERY_HH
#define LIBBYUUML_QUERY_HH

#include "byuuML.hpp"

namespace byuuML {
  class missing_node_exception : public std::exception {
  public:
    const char* what() const throw() override { return "no such node"; }
  };
  class cursor {
    node::const_iterator finder;
    void find_node(std::string& name) {
      while(!finder.is_over()) {
        if(finder->get_name() == name) break;
        ++finder;
      }
    }
  public:
    cursor(const std::unique_ptr<node[]>& buf,
           node::index starting_node)
      : cursor(node::const_iterator(buf, starting_node)) {}
    cursor(const std::unique_ptr<node[]>& buf,
           node::index starting_node,
           std::string initial)
      : cursor(node::const_iterator(buf, starting_node), initial) {}
    template<typename ...X>
    cursor(const std::unique_ptr<node[]>& buf,
           node::index starting_node,
           std::string initial,
           X&&... params)
      : cursor(node::const_iterator(buf, starting_node), initial,
               std::forward<X>(params)...) {}
    cursor(node::const_iterator it)
      : finder(it) {}
    cursor(node::const_iterator it,
           std::string initial)
      : cursor(it) {
      find_node(initial);
    }
    template<typename ...X>
    cursor(node::const_iterator it,
           std::string initial,
           X&&... params)
      : cursor(it, initial) {
      *this = query(std::forward<X>(params)...);
    }
    operator bool() const {
      return !finder.is_over();
    }
    cursor query(std::string name) const {
      if(!finder.is_over())
        return cursor(node::const_iterator(finder.get_node_buffer(),
                                           finder->get_child_index()), name);
      else
        return *this;
    }
    template<typename ...X>
    cursor query(std::string name, X&&... params) const {
      return query(name).query(std::forward<X>(params)...);
    }
    cursor operator[](std::string wat) const { return query(wat); }
    cursor operator[](const char* wat) const { return query(wat); }
    cursor sibling(std::string name) const {
      if(!*this) return *this;
      cursor ret(finder);
      ++ret.finder;
      ret.find_node(name);
      return ret;
    }
    const node& get_node() const {
      if(finder.is_over()) throw missing_node_exception();
      return *finder;
    }
    const std::string& name() const { return get_node().get_name(); }
    const std::string& data() const { return get_node().get_data(); }
    const std::string& data(const std::string& def) const {
      if(finder.is_over()) return def;
      else return finder->get_data();
    }
    template<class T> T value() const { return T(data()); }
    template<class T> T value(const T& def) const {
      if(finder.is_over()) return def;
      else return value<T>();
    }
    // for when we're acting like an iterator
    cursor begin() const { return *this; }
    cursor end() const { return cursor(node::const_iterator(finder.get_node_buffer(), node::SENTINEL_INDEX)); }
    cursor& operator++() {
      if(*this) *this = this->sibling(name());
      return *this;
    }
    bool operator==(const cursor& other) const {
      return finder == other.finder;
    }
    bool operator!=(const cursor& other) const {
      return finder != other.finder;
    }
    cursor& operator*() { return *this; }
    const cursor& operator*() const { return *this; }
    cursor* operator->() { return this; }
    const cursor* operator->() const { return this; }
    // for debugging
    node::const_iterator get_iterator() const { return finder; }
  };
  template<typename ...X> cursor node::query(const document& document, X&&...params) const {
    return byuuML::cursor(document.get_node_buffer(),
                          this - document.get_node_buffer().get(),
                          std::forward<X>(params)...);
  }
  template<typename ...X> cursor document::query(X&&... params) const {
    return byuuML::cursor(get_node_buffer(), 0,
                          std::forward<X>(params)...);
  }
  inline cursor document::operator[](std::string wat) const { return query(wat); }
  inline cursor document::operator[](const char* wat) const { return query(wat); }
  template<typename ...X> cursor node_in_document::query(X&&... params) const {
    return byuuML::cursor(node_buffer, &node_ref - node_buffer.get(),
                          std::forward<X>(params)...);
  }
  inline cursor node_in_document::operator[](std::string wat) const { return query(wat); }
  inline cursor node_in_document::operator[](const char* wat) const { return query(wat); }
  template<> inline int cursor::value() const {
    std::size_t pos = 0;
    auto& d = data();
    auto ret = std::stoi(d, &pos, 0);
    if(pos != d.length()) throw std::invalid_argument("not an integer");
    return ret;
  }
  template<> inline long cursor::value() const {
    std::size_t pos = 0;
    auto& d = data();
    auto ret = std::stol(d, &pos, 0);
    if(pos != d.length()) throw std::invalid_argument("not an integer");
    return ret;
  }
  template<> inline unsigned long cursor::value() const {
    std::size_t pos = 0;
    auto& d = data();
    auto ret = std::stoul(d, &pos, 0);
    if(pos != d.length()) throw std::invalid_argument("not an integer");
    return ret;
  }
  template<> inline long long cursor::value() const {
    std::size_t pos = 0;
    auto& d = data();
    auto ret = std::stoll(d, &pos, 0);
    if(pos != d.length()) throw std::invalid_argument("not an integer");
    return ret;
  }
  template<> inline unsigned long long cursor::value() const {
    std::size_t pos = 0;
    auto& d = data();
    auto ret = std::stoull(d, &pos, 0);
    if(pos != d.length()) throw std::invalid_argument("not an integer");
    return ret;
  }
  template<> inline float cursor::value() const {
    std::size_t pos = 0;
    auto& d = data();
    auto ret = std::stof(d, &pos);
    if(pos != d.length()) throw std::invalid_argument("not a number");
    return ret;
  }
  template<> inline double cursor::value() const {
    std::size_t pos = 0;
    auto& d = data();
    auto ret = std::stod(d, &pos);
    if(pos != d.length()) throw std::invalid_argument("not a number");
    return ret;
  }
  template<> inline long double cursor::value() const {
    std::size_t pos = 0;
    auto& d = data();
    auto ret = std::stold(d, &pos);
    if(pos != d.length()) throw std::invalid_argument("not a number");
    return ret;
  }
}

#endif
