/* List of changes as per Zlib license terms:
   - Filenames modified to match bsnes convention
*/
#ifndef LIBBYUUMLHH
#define LIBBYUUMLHH

#include <memory>
#include <string>

/*
  tl;dr: implement byuuML::reader, pass one to the constructor of
  byuuML::document, use iterators to access the document tree.
 */

namespace byuuML {
  class cursor;
  class document;
  class node_in_document;
  class node {
  public:
    /*
      This limits us to ~4 billion nodes on most machines, and 65535 on some.
      I don't see this as problematic. If you do, change this to size_t.
    */
    typedef unsigned int index;
    static constexpr index SENTINEL_INDEX = ~index(0);
  //private:
    std::string name;
    std::string data;
    unsigned int sibling, child;
  public:
    class const_iterator {
      const std::unique_ptr<node[]>* node_buffer;
      index node_index;
    public:
      const_iterator(const std::unique_ptr<node[]>& _node_buffer,
                     index _node_index)
        : node_buffer(&_node_buffer), node_index(_node_index) {}
      const_iterator& operator++() {
        node_index = (*node_buffer)[node_index].sibling;
        return *this;
      }
      bool operator==(const const_iterator& other) const {
        return node_buffer == other.node_buffer
          && node_index == other.node_index;
      }
      bool operator!=(const const_iterator& other) const {
        return !(*this == other);
      }
      const node& operator*() const { return (*node_buffer)[node_index]; }
      const node* operator->() const { return &(*node_buffer)[node_index]; }
      // not a standard part of being an iterator
      bool is_over() const { return node_index == SENTINEL_INDEX; }
      index get_node_index() const { return node_index; }
      const std::unique_ptr<node[]>& get_node_buffer() const {
        return *node_buffer;
      }
    };
    node() : sibling(SENTINEL_INDEX), child(SENTINEL_INDEX) {}
    node(std::string node_name, std::string node_data, index node_sibling, index node_child)
      : name(node_name), data(node_data), sibling(node_sibling), child(node_child) {}
    bool has_children() const { return child != SENTINEL_INDEX; }
    const std::string& get_name() const { return name; }
    const std::string& get_data() const { return data; }
    index get_sibling_index() const { return sibling; }
    index get_child_index() const { return child; }
    template <typename ...X> cursor query(const document& document, X&&... params) const;
    node_in_document in_document(const document& document) const;
    node_in_document in_document(const std::unique_ptr<node[]>& node_buffer) const;
  };
  class reader {
  public:
    // Signal EOF by making begin == end
    virtual void read_more(const char*& begin, const char*& end) = 0;
  };
  class document {
    std::unique_ptr<node[]> node_buffer;
  public:
    // will throw a std::string on failure
    // max_depth is approximate, it may sometimes be exceeded by one
    document(reader&, size_t max_depth = 50);
    const std::unique_ptr<node[]>& get_node_buffer() const { return node_buffer; }
    node::const_iterator begin() const {
      return node::const_iterator(node_buffer, 0);
    }
    node::const_iterator cbegin() const { return begin(); }
    node::const_iterator end() const {
      return node::const_iterator(node_buffer, node::SENTINEL_INDEX);
    }
    node::const_iterator cend() const { return end(); }
    template <typename ...X> cursor query(X&&... params) const;
    cursor operator[](std::string wat) const;
    cursor operator[](const char* wat) const;
  };
  class node_in_document {
    const std::unique_ptr<node[]>& node_buffer;
    const node& node_ref;
  public:
    node_in_document(const std::unique_ptr<node[]>& doc_node_buffer,
                     const node& doc_node_ref)
      : node_buffer(doc_node_buffer), node_ref(doc_node_ref) {}
    node_in_document(const document& document,
                     const node& doc_node_ref)
      : node_buffer(document.get_node_buffer()), node_ref(doc_node_ref) {}
    node_in_document(const node& doc_node_ref,
                     const std::unique_ptr<node[]>& doc_node_buffer)
      : node_buffer(doc_node_buffer), node_ref(doc_node_ref) {}
    node_in_document(const node& doc_node_ref,
                     const document& document)
      : node_buffer(document.get_node_buffer()), node_ref(doc_node_ref) {}
    node::const_iterator begin() const {
      return node::const_iterator(node_buffer, node_ref.get_child_index());
    }
    node::const_iterator cbegin() const { return begin(); }
    node::const_iterator end() const {
      return node::const_iterator(node_buffer, node::SENTINEL_INDEX);
    }
    node::const_iterator cend() const { return end(); }
    template <typename ...X> cursor query(X&&... params) const;
    cursor operator[](std::string wat) const;
    cursor operator[](const char* wat) const;
  };
  inline node_in_document node::in_document(const document& document) const {
    return node_in_document(document, *this);
  }
  inline node_in_document node::in_document(const std::unique_ptr<node[]>& node_buffer) const {
    return node_in_document(node_buffer, *this);
  }
}

#endif
