#include "emulator.hpp"

namespace nall::BML {

auto unserialize(const string& markup, string_view spacing) -> Markup::Node {
  shared_pointer<ManagedNode> node(new ManagedNode);
  try {
    node->parse(markup, spacing);
  } catch(const char* error) {
    node.reset();
  }
  return (Markup::SharedNode&)node;
}

string serialize(const Markup::Node& node, string_view spacing, unsigned depth) {
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
