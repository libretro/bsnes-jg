#pragma once

namespace Markup {

struct Node;
struct ManagedNode;
using SharedNode = nall::shared_pointer<ManagedNode>;

struct ManagedNode {
  ManagedNode() = default;
  ManagedNode(const nall::string& name) : _name(name) {}
  ManagedNode(const nall::string& name, const nall::string& value) : _name(name), _value(value) {}

  nall::shared_pointer<ManagedNode> clone() const {
    nall::shared_pointer<ManagedNode> clone{new ManagedNode(_name, _value)};
    for(auto& child : _children) {
      clone->_children.append(child->clone());
    }
    return clone;
  }

  void copy(nall::shared_pointer<ManagedNode> source) {
    _name = source->_name;
    _value = source->_value;
    _metadata = source->_metadata;
    _children.reset();
    for(auto child : source->_children) {
      _children.append(child->clone());
    }
  }

protected:
  nall::string _name;
  nall::string _value;
  uintptr_t _metadata = 0;
  nall::vector<nall::shared_pointer<ManagedNode>> _children;

  inline bool _evaluate(nall::string query) const;
  inline nall::vector<Node> _find(const nall::string& query) const;
  inline Node _lookup(const nall::string& path) const;
  inline Node _create(const nall::string& path);

  friend class Node;
};

struct Node {
  Node() : shared(new ManagedNode) {}
  Node(const nall::shared_pointer<ManagedNode>& source) : shared(source ? source : new ManagedNode) {}
  Node(const nall::string& name) : shared(new ManagedNode(name)) {}
  Node(const nall::string& name, const nall::string& value) : shared(new ManagedNode(name, value)) {}

  bool unique() const { return shared.unique(); }
  Node clone() const { return shared->clone(); }
  void copy(Node source) { return shared->copy(source.shared); }

  explicit operator bool() const { return shared->_name || shared->_children; }
  nall::string name() const { return shared->_name; }
  nall::string value() const { return shared->_value; }

  bool value(nall::string& target) const { if(shared) target = string(); return (bool)shared; }
  bool value(bool& target) const { if(shared) target = boolean(); return (bool)shared; }
  bool value(int& target) const { if(shared) target = integer(); return (bool)shared; }
  bool value(unsigned& target) const { if(shared) target = natural(); return (bool)shared; }
  bool value(double& target) const { if(shared) target = real(); return (bool)shared; }

  nall::string text() const { return value().strip(); }
  nall::string string() const { return value().strip(); }
  bool boolean() const { return text() == "true"; }
  int64_t integer() const { return text().integer(); }
  uint64_t natural() const { return text().natural(); }
  double real() const { return text().real(); }

  nall::string text(const nall::string& fallback) const { return bool(*this) ? text() : fallback; }
  nall::string string(const nall::string& fallback) const { return bool(*this) ? string() : fallback; }
  bool boolean(bool fallback) const { return bool(*this) ? boolean() : fallback; }
  int64_t integer(int64_t fallback) const { return bool(*this) ? integer() : fallback; }
  uint64_t natural(uint64_t fallback) const { return bool(*this) ? natural() : fallback; }
  double real(double fallback) const { return bool(*this) ? real() : fallback; }

  Node& setName(const nall::string& name = "") { shared->_name = name; return *this; }
  Node& setValue(const nall::string& value = "") { shared->_value = value; return *this; }

  void reset() { shared->_children.reset(); }
  unsigned size() const { return shared->_children.size(); }

  void prepend(const Node& node) { shared->_children.prepend(node.shared); }
  void append(const Node& node) { shared->_children.append(node.shared); }
  bool remove(const Node& node) {
    for(auto n : nall::range(size())) {
      if(node.shared == shared->_children[n]) {
        return shared->_children.remove(n), true;
      }
    }
    return false;
  }

  bool insert(unsigned position, const Node& node) {
    if(position > size()) return false;  //used > instead of >= to allow indexed-equivalent of append()
    return shared->_children.insert(position, node.shared), true;
  }

  bool remove(unsigned position) {
    if(position >= size()) return false;
    return shared->_children.remove(position), true;
  }

  bool swap(unsigned x, unsigned y) {
    if(x >= size() || y >= size()) return false;
    return std::swap(shared->_children[x], shared->_children[y]), true;
  }

  Node operator[](int position) {
    if(position >= size()) return {};
    return shared->_children[position];
  }

  Node operator[](const nall::string& path) const { return shared->_lookup(path); }
  Node operator()(const nall::string& path) { return shared->_create(path); }
  nall::vector<Node> find(const nall::string& query) const { return shared->_find(query); }

  struct iterator {
    Node operator*() { return {source.shared->_children[position]}; }
    bool operator!=(const iterator& source) const { return position != source.position; }
    iterator& operator++() { return position++, *this; }
    iterator(const Node& source, unsigned position) : source(source), position(position) {}

  private:
    const Node& source;
    unsigned position;
  };

  iterator begin() const { return iterator(*this, 0); }
  iterator end() const { return iterator(*this, size()); }

protected:
  nall::shared_pointer<ManagedNode> shared;
};

bool ManagedNode::_evaluate(nall::string query) const {
  if(!query) return true;

  for(auto& rule : query.split(",")) {
    enum class Comparator : unsigned { ID, EQ, NE, LT, LE, GT, GE };
    auto comparator = Comparator::ID;
         if(rule.match("*!=*")) comparator = Comparator::NE;
    else if(rule.match("*<=*")) comparator = Comparator::LE;
    else if(rule.match("*>=*")) comparator = Comparator::GE;
    else if(rule.match ("*=*")) comparator = Comparator::EQ;
    else if(rule.match ("*<*")) comparator = Comparator::LT;
    else if(rule.match ("*>*")) comparator = Comparator::GT;

    if(comparator == Comparator::ID) {
      if(_find(rule).size()) continue;
      return false;
    }

    nall::vector<nall::string> side;
    switch(comparator) {
    case Comparator::EQ: side = rule.split ("=", 1L); break;
    case Comparator::NE: side = rule.split("!=", 1L); break;
    case Comparator::LT: side = rule.split ("<", 1L); break;
    case Comparator::LE: side = rule.split("<=", 1L); break;
    case Comparator::GT: side = rule.split (">", 1L); break;
    case Comparator::GE: side = rule.split(">=", 1L); break;
    }

    nall::string data = nall::string{_value}.strip();
    if(side(0)) {
      auto result = _find(side(0));
      if(result.size() == 0) return false;
      data = result[0].text();  //strips whitespace so rules can match without requiring it
    }

    switch(comparator) {
    case Comparator::EQ: if(data.match(side(1)) ==  true)      continue; break;
    case Comparator::NE: if(data.match(side(1)) == false)      continue; break;
    case Comparator::LT: if(data.natural()  < side(1).natural()) continue; break;
    case Comparator::LE: if(data.natural() <= side(1).natural()) continue; break;
    case Comparator::GT: if(data.natural()  > side(1).natural()) continue; break;
    case Comparator::GE: if(data.natural() >= side(1).natural()) continue; break;
    }

    return false;
  }

  return true;
}

nall::vector<Node> ManagedNode::_find(const nall::string& query) const {
  nall::vector<Node> result;

  auto path = query.split("/");
  nall::string name = path.take(0), rule;
  unsigned lo = 0u, hi = ~0u;

  if(name.match("*[*]")) {
    auto p = name.trimRight("]", 1L).split("[", 1L);
    name = p(0);
    if(p(1).find("-")) {
      p = p(1).split("-", 1L);
      lo = !p(0) ?  0u : p(0).natural();
      hi = !p(1) ? ~0u : p(1).natural();
    } else {
      lo = hi = p(1).natural();
    }
  }

  if(name.match("*(*)")) {
    auto p = name.trimRight(")", 1L).split("(", 1L);
    name = p(0);
    rule = p(1);
  }

  unsigned position = 0;
  for(auto& node : _children) {
    if(!node->_name.match(name)) continue;
    if(!node->_evaluate(rule)) continue;

    bool inrange = position >= lo && position <= hi;
    position++;
    if(!inrange) continue;

    if(path.size() == 0) {
      result.append(node);
    } else for(auto& item : node->_find(path.merge("/"))) {
      result.append(item);
    }
  }

  return result;
}

//operator[](nall::string)
Node ManagedNode::_lookup(const nall::string& path) const {
  auto result = _find(path);
  return result ? result[0] : Node{};

/*//faster, but cannot search
  if(auto position = path.find("/")) {
    auto name = nall::slice(path, 0, *position);
    for(auto& node : _children) {
      if(name == node->_name) {
        return node->_lookup(nall::slice(path, *position + 1));
      }
    }
  } else for(auto& node : _children) {
    if(path == node->_name) return node;
  }
  return {};
*/
}

Node ManagedNode::_create(const nall::string& path) {
  if(auto position = path.find("/")) {
    auto name = nall::slice(path, 0, *position);
    for(auto& node : _children) {
      if(name == node->_name) {
        return node->_create(nall::slice(path, *position + 1));
      }
    }
    _children.append(new ManagedNode(name));
    return _children.right()->_create(nall::slice(path, *position + 1));
  }
  for(auto& node : _children) {
    if(path == node->_name) return node;
  }
  _children.append(new ManagedNode(path));
  return _children.right();
}

}

namespace BML {

//metadata is used to store nesting level

struct ManagedNode : Markup::ManagedNode {
protected:
  //test to verify if a valid character for a node name
  bool valid(char p) const {  //A-Z, a-z, 0-9, -.
    return p - 'A' < 26u || p - 'a' < 26u || p - '0' < 10u || p - '-' < 2u;
  }

  //determine indentation level, without incrementing pointer
  unsigned readDepth(const char* p) {
    unsigned depth = 0;
    while(p[depth] == '\t' || p[depth] == ' ') depth++;
    return depth;
  }

  //determine indentation level
  unsigned parseDepth(const char*& p) {
    unsigned depth = readDepth(p);
    p += depth;
    return depth;
  }

  //read name
  void parseName(const char*& p) {
    unsigned length = 0;
    while(valid(p[length])) length++;
    if(length == 0) throw "Invalid node name";
    _name = nall::slice(p, 0, length);
    p += length;
  }

  void parseData(const char*& p, nall::string_view spacing) {
    if(*p == '=' && *(p + 1) == '\"') {
      unsigned length = 2;
      while(p[length] && p[length] != '\n' && p[length] != '\"') length++;
      if(p[length] != '\"') throw "Unescaped value";
      _value = {nall::slice(p, 2, length - 2), "\n"};
      p += length + 1;
    } else if(*p == '=') {
      unsigned length = 1;
      while(p[length] && p[length] != '\n' && p[length] != '\"' && p[length] != ' ') length++;
      if(p[length] == '\"') throw "Illegal character in value";
      _value = {nall::slice(p, 1, length - 1), "\n"};
      p += length;
    } else if(*p == ':') {
      unsigned length = 1;
      while(p[length] && p[length] != '\n') length++;
      _value = {nall::slice(p, 1, length - 1).trimLeft(spacing, 1L), "\n"};
      p += length;
    }
  }

  //read all attributes for a node
  void parseAttributes(const char*& p, nall::string_view spacing) {
    while(*p && *p != '\n') {
      if(*p != ' ') throw "Invalid node name";
      while(*p == ' ') p++;  //skip excess spaces
      if(*(p + 0) == '/' && *(p + 1) == '/') break;  //skip comments

      nall::shared_pointer<ManagedNode> node(new ManagedNode);
      unsigned length = 0;
      while(valid(p[length])) length++;
      if(length == 0) throw "Invalid attribute name";
      node->_name = nall::slice(p, 0, length);
      node->parseData(p += length, spacing);
      node->_value.trimRight("\n", 1L);
      _children.append(node);
    }
  }

  //read a node and all of its child nodes
  void parseNode(const nall::vector<nall::string>& text, unsigned& y, nall::string_view spacing) {
    const char* p = text[y++];
    _metadata = parseDepth(p);
    parseName(p);
    parseData(p, spacing);
    parseAttributes(p, spacing);

    while(y < text.size()) {
      unsigned depth = readDepth(text[y]);
      if(depth <= _metadata) break;

      if(text[y][depth] == ':') {
        _value.append(nall::slice(text[y++], depth + 1).trimLeft(spacing, 1L), "\n");
        continue;
      }

      nall::shared_pointer<ManagedNode> node(new ManagedNode);
      node->parseNode(text, y, spacing);
      _children.append(node);
    }

    _value.trimRight("\n", 1L);
  }

  //read top-level nodes
  void parse(nall::string document, nall::string_view spacing) {
    //in order to simplify the parsing logic; we do an initial pass to normalize the data
    //the below code will turn '\r\n' into '\n'; skip empty lines; and skip comment lines
    char* p = document.get(), *output = p;
    while(*p) {
      char* origin = p;
      bool empty = true;
      while(*p) {
        //scan for first non-whitespace character. if it's a line feed or comment; skip the line
        if(p[0] == ' ' || p[0] == '\t') { p++; continue; }
        empty = p[0] == '\r' || p[0] == '\n' || (p[0] == '/' && p[1] == '/');
        break;
      }
      while(*p) {
        if(p[0] == '\r') p[0] = '\n';  //turns '\r\n' into '\n\n' (second '\n' will be skipped)
        if(*p++ == '\n') break;        //include '\n' in the output to be copied
      }
      if(empty) continue;

      nall::memory::move(output, origin, p - origin);
      output += p - origin;
    }
    document.resize(document.size() - (p - output)).trimRight("\n");
    if(document.size() == 0) return;  //empty document

    auto text = document.split("\n");
    unsigned y = 0;
    while(y < text.size()) {
      nall::shared_pointer<ManagedNode> node(new ManagedNode);
      node->parseNode(text, y, spacing);
      if(node->_metadata > 0) throw "Root nodes cannot be indented";
      _children.append(node);
    }
  }

  friend Markup::Node unserialize(const nall::string&, nall::string_view);
};

Markup::Node unserialize(const nall::string& markup, nall::string_view spacing = {});
nall::string serialize(const Markup::Node& node, nall::string_view spacing = {}, unsigned depth = 0);


std::string gendoc(std::string dbpath, std::string parent, std::string child, std::string match);
std::string search(std::string text, std::vector<std::string> terms);
std::vector<std::string> searchList(std::string text, std::vector<std::string> terms);

}
