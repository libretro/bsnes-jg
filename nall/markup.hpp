#pragma once

namespace nall::Markup {

struct Node;
struct ManagedNode;
using SharedNode = shared_pointer<ManagedNode>;

struct ManagedNode {
  ManagedNode() = default;
  ManagedNode(const string& name) : _name(name) {}
  ManagedNode(const string& name, const string& value) : _name(name), _value(value) {}

  auto clone() const -> SharedNode {
    SharedNode clone{new ManagedNode(_name, _value)};
    for(auto& child : _children) {
      clone->_children.append(child->clone());
    }
    return clone;
  }

  auto copy(SharedNode source) -> void {
    _name = source->_name;
    _value = source->_value;
    _metadata = source->_metadata;
    _children.reset();
    for(auto child : source->_children) {
      _children.append(child->clone());
    }
  }

protected:
  string _name;
  string _value;
  uintptr _metadata = 0;
  vector<SharedNode> _children;

  inline auto _evaluate(string query) const -> bool;
  inline auto _find(const string& query) const -> vector<Node>;
  inline auto _lookup(const string& path) const -> Node;
  inline auto _create(const string& path) -> Node;

  friend class Node;
};

struct Node {
  Node() : shared(new ManagedNode) {}
  Node(const SharedNode& source) : shared(source ? source : new ManagedNode) {}
  Node(const nall::string& name) : shared(new ManagedNode(name)) {}
  Node(const nall::string& name, const nall::string& value) : shared(new ManagedNode(name, value)) {}

  auto unique() const -> bool { return shared.unique(); }
  auto clone() const -> Node { return shared->clone(); }
  auto copy(Node source) -> void { return shared->copy(source.shared); }

  explicit operator bool() const { return shared->_name || shared->_children; }
  auto name() const -> nall::string { return shared->_name; }
  auto value() const -> nall::string { return shared->_value; }

  auto value(nall::string& target) const -> bool { if(shared) target = string(); return (bool)shared; }
  auto value(bool& target) const -> bool { if(shared) target = boolean(); return (bool)shared; }
  auto value(int& target) const -> bool { if(shared) target = integer(); return (bool)shared; }
  auto value(uint& target) const -> bool { if(shared) target = natural(); return (bool)shared; }
  auto value(double& target) const -> bool { if(shared) target = real(); return (bool)shared; }

  auto text() const -> nall::string { return value().strip(); }
  auto string() const -> nall::string { return value().strip(); }
  auto boolean() const -> bool { return text() == "true"; }
  auto integer() const -> int64_t { return text().integer(); }
  auto natural() const -> uint64_t { return text().natural(); }
  auto real() const -> double { return text().real(); }

  auto text(const nall::string& fallback) const -> nall::string { return bool(*this) ? text() : fallback; }
  auto string(const nall::string& fallback) const -> nall::string { return bool(*this) ? string() : fallback; }
  auto boolean(bool fallback) const -> bool { return bool(*this) ? boolean() : fallback; }
  auto integer(int64_t fallback) const -> int64_t { return bool(*this) ? integer() : fallback; }
  auto natural(uint64_t fallback) const -> uint64_t { return bool(*this) ? natural() : fallback; }
  auto real(double fallback) const -> double { return bool(*this) ? real() : fallback; }

  auto setName(const nall::string& name = "") -> Node& { shared->_name = name; return *this; }
  auto setValue(const nall::string& value = "") -> Node& { shared->_value = value; return *this; }

  auto reset() -> void { shared->_children.reset(); }
  auto size() const -> uint { return shared->_children.size(); }

  auto prepend(const Node& node) -> void { shared->_children.prepend(node.shared); }
  auto append(const Node& node) -> void { shared->_children.append(node.shared); }
  auto remove(const Node& node) -> bool {
    for(auto n : range(size())) {
      if(node.shared == shared->_children[n]) {
        return shared->_children.remove(n), true;
      }
    }
    return false;
  }

  auto insert(uint position, const Node& node) -> bool {
    if(position > size()) return false;  //used > instead of >= to allow indexed-equivalent of append()
    return shared->_children.insert(position, node.shared), true;
  }

  auto remove(uint position) -> bool {
    if(position >= size()) return false;
    return shared->_children.remove(position), true;
  }

  auto swap(uint x, uint y) -> bool {
    if(x >= size() || y >= size()) return false;
    return std::swap(shared->_children[x], shared->_children[y]), true;
  }

  auto sort(function<bool (Node, Node)> comparator = [](auto x, auto y) {
    return nall::string::compare(x.shared->_name, y.shared->_name) < 0;
  }) -> void {
    nall::sort(shared->_children.data(), shared->_children.size(), [&](auto x, auto y) {
      return comparator(x, y);  //this call converts SharedNode objects to Node objects
    });
  }

  auto operator[](int position) -> Node {
    if(position >= size()) return {};
    return shared->_children[position];
  }

  auto operator[](const nall::string& path) const -> Node { return shared->_lookup(path); }
  auto operator()(const nall::string& path) -> Node { return shared->_create(path); }
  auto find(const nall::string& query) const -> vector<Node> { return shared->_find(query); }

  struct iterator {
    auto operator*() -> Node { return {source.shared->_children[position]}; }
    auto operator!=(const iterator& source) const -> bool { return position != source.position; }
    auto operator++() -> iterator& { return position++, *this; }
    iterator(const Node& source, uint position) : source(source), position(position) {}

  private:
    const Node& source;
    uint position;
  };

  auto begin() const -> iterator { return iterator(*this, 0); }
  auto end() const -> iterator { return iterator(*this, size()); }

protected:
  SharedNode shared;
};

auto ManagedNode::_evaluate(string query) const -> bool {
  if(!query) return true;

  for(auto& rule : query.split(",")) {
    enum class Comparator : uint { ID, EQ, NE, LT, LE, GT, GE };
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

    vector<string> side;
    switch(comparator) {
    case Comparator::EQ: side = rule.split ("=", 1L); break;
    case Comparator::NE: side = rule.split("!=", 1L); break;
    case Comparator::LT: side = rule.split ("<", 1L); break;
    case Comparator::LE: side = rule.split("<=", 1L); break;
    case Comparator::GT: side = rule.split (">", 1L); break;
    case Comparator::GE: side = rule.split(">=", 1L); break;
    }

    string data = string{_value}.strip();
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

auto ManagedNode::_find(const string& query) const -> vector<Node> {
  vector<Node> result;

  auto path = query.split("/");
  string name = path.take(0), rule;
  uint lo = 0u, hi = ~0u;

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

  uint position = 0;
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

//operator[](string)
auto ManagedNode::_lookup(const string& path) const -> Node {
  auto result = _find(path);
  return result ? result[0] : Node{};

/*//faster, but cannot search
  if(auto position = path.find("/")) {
    auto name = slice(path, 0, *position);
    for(auto& node : _children) {
      if(name == node->_name) {
        return node->_lookup(slice(path, *position + 1));
      }
    }
  } else for(auto& node : _children) {
    if(path == node->_name) return node;
  }
  return {};
*/
}

auto ManagedNode::_create(const string& path) -> Node {
  if(auto position = path.find("/")) {
    auto name = slice(path, 0, *position);
    for(auto& node : _children) {
      if(name == node->_name) {
        return node->_create(slice(path, *position + 1));
      }
    }
    _children.append(new ManagedNode(name));
    return _children.right()->_create(slice(path, *position + 1));
  }
  for(auto& node : _children) {
    if(path == node->_name) return node;
  }
  _children.append(new ManagedNode(path));
  return _children.right();
}

}

namespace nall::BML {

//metadata is used to store nesting level

struct ManagedNode;
using SharedNode = shared_pointer<ManagedNode>;

struct ManagedNode : Markup::ManagedNode {
protected:
  //test to verify if a valid character for a node name
  auto valid(char p) const -> bool {  //A-Z, a-z, 0-9, -.
    return p - 'A' < 26u || p - 'a' < 26u || p - '0' < 10u || p - '-' < 2u;
  }

  //determine indentation level, without incrementing pointer
  auto readDepth(const char* p) -> uint {
    uint depth = 0;
    while(p[depth] == '\t' || p[depth] == ' ') depth++;
    return depth;
  }

  //determine indentation level
  auto parseDepth(const char*& p) -> uint {
    uint depth = readDepth(p);
    p += depth;
    return depth;
  }

  //read name
  auto parseName(const char*& p) -> void {
    uint length = 0;
    while(valid(p[length])) length++;
    if(length == 0) throw "Invalid node name";
    _name = slice(p, 0, length);
    p += length;
  }

  auto parseData(const char*& p, string_view spacing) -> void {
    if(*p == '=' && *(p + 1) == '\"') {
      uint length = 2;
      while(p[length] && p[length] != '\n' && p[length] != '\"') length++;
      if(p[length] != '\"') throw "Unescaped value";
      _value = {slice(p, 2, length - 2), "\n"};
      p += length + 1;
    } else if(*p == '=') {
      uint length = 1;
      while(p[length] && p[length] != '\n' && p[length] != '\"' && p[length] != ' ') length++;
      if(p[length] == '\"') throw "Illegal character in value";
      _value = {slice(p, 1, length - 1), "\n"};
      p += length;
    } else if(*p == ':') {
      uint length = 1;
      while(p[length] && p[length] != '\n') length++;
      _value = {slice(p, 1, length - 1).trimLeft(spacing, 1L), "\n"};
      p += length;
    }
  }

  //read all attributes for a node
  auto parseAttributes(const char*& p, string_view spacing) -> void {
    while(*p && *p != '\n') {
      if(*p != ' ') throw "Invalid node name";
      while(*p == ' ') p++;  //skip excess spaces
      if(*(p + 0) == '/' && *(p + 1) == '/') break;  //skip comments

      SharedNode node(new ManagedNode);
      uint length = 0;
      while(valid(p[length])) length++;
      if(length == 0) throw "Invalid attribute name";
      node->_name = slice(p, 0, length);
      node->parseData(p += length, spacing);
      node->_value.trimRight("\n", 1L);
      _children.append(node);
    }
  }

  //read a node and all of its child nodes
  auto parseNode(const vector<string>& text, uint& y, string_view spacing) -> void {
    const char* p = text[y++];
    _metadata = parseDepth(p);
    parseName(p);
    parseData(p, spacing);
    parseAttributes(p, spacing);

    while(y < text.size()) {
      uint depth = readDepth(text[y]);
      if(depth <= _metadata) break;

      if(text[y][depth] == ':') {
        _value.append(slice(text[y++], depth + 1).trimLeft(spacing, 1L), "\n");
        continue;
      }

      SharedNode node(new ManagedNode);
      node->parseNode(text, y, spacing);
      _children.append(node);
    }

    _value.trimRight("\n", 1L);
  }

  //read top-level nodes
  auto parse(string document, string_view spacing) -> void {
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

      memory::move(output, origin, p - origin);
      output += p - origin;
    }
    document.resize(document.size() - (p - output)).trimRight("\n");
    if(document.size() == 0) return;  //empty document

    auto text = document.split("\n");
    uint y = 0;
    while(y < text.size()) {
      SharedNode node(new ManagedNode);
      node->parseNode(text, y, spacing);
      if(node->_metadata > 0) throw "Root nodes cannot be indented";
      _children.append(node);
    }
  }

  friend auto unserialize(const string&, string_view) -> Markup::Node;
};

inline auto unserialize(const string& markup, string_view spacing = {}) -> Markup::Node {
  SharedNode node(new ManagedNode);
  try {
    node->parse(markup, spacing);
  } catch(const char* error) {
    node.reset();
  }
  return (Markup::SharedNode&)node;
}

inline auto serialize(const Markup::Node& node, string_view spacing = {}, uint depth = 0) -> string {
  if(!node.name()) {
    string result;
    for(auto leaf : node) {
      result.append(serialize(leaf, spacing, depth));
    }
    return result;
  }

  string padding;
  padding.resize(depth * 2);
  padding.fill(' ');

  vector<string> lines;
  if(auto value = node.value()) lines = value.split("\n");

  string result;
  result.append(padding);
  result.append(node.name());
  if(lines.size() == 1) result.append(":", spacing, lines[0]);
  result.append("\n");
  if(lines.size() > 1) {
    padding.append("  ");
    for(auto& line : lines) {
      result.append(padding, ":", spacing, line, "\n");
    }
  }
  for(auto leaf : node) {
    result.append(serialize(leaf, spacing, depth + 1));
  }
  return result;
}

}
