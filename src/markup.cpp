#include "emulator.hpp"

namespace BML {

Markup::Node unserialize(const nall::string& markup, nall::string_view spacing) {
  nall::shared_pointer<ManagedNode> node(new ManagedNode);
  try {
    node->parse(markup, spacing);
  } catch(const char* error) {
    node.reset();
  }
  return (Markup::SharedNode&)node;
}

nall::string serialize(const Markup::Node& node, nall::string_view spacing, unsigned depth) {
  if(!node.name()) {
    nall::string result;
    for(auto leaf : node) {
      result.append(serialize(leaf, spacing, depth));
    }
    return result;
  }

  nall::string padding;
  padding.resize(depth * 2);
  padding.fill(' ');

  nall::vector<nall::string> lines;
  if(auto value = node.value()) lines = value.split("\n");

  nall::string result;
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
