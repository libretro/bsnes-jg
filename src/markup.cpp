#include <iostream>
#include <sstream>

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


class streamreader : public byuuML::reader {
    char buf[512];
    std::istream& is;
public:
    streamreader(std::istream& is) : is(is) {}
    void read_more(const char*& begin, const char*& end) override {
        begin = buf;
        if(!is) {
            end = begin;
            return;
        }
        is.read(buf, sizeof(buf));
        end = begin + is.gcount();
    }
};

void dumpnode(std::ostream& out, const byuuML::document& document, const byuuML::node& node, int indent_level = 0) {

    for (int n = 0; n < indent_level; ++n) out << "  ";
    out << node.name;
    if (!node.data.empty()) out << ":" << node.data;
    out << "\n";

    for (auto&& child : byuuML::node_in_document(node, document)) {
        dumpnode(out, document, child, indent_level + 1);
    }
}

std::string gendoc(std::string docpath, std::string parent, std::string child, std::string val) {
    std::ifstream stream(docpath, std::ios::in | std::ios::binary);
    if (!stream.is_open()) return {};

    std::istream& is = stream;

    streamreader bmlreader(is);
    byuuML::document doc(bmlreader);

    for (auto&& node : doc) {
        byuuML::cursor p0 = node.query(doc, parent);
        if (p0) {
            byuuML::cursor p1 = node.query(doc, parent, child);
            if (p1) {
                std::string str(p1.value<std::string>());
                str.erase(0, str.find_first_not_of(' '));
                if (str == val) {
                    if (node.name != parent) continue;
                    std::stringstream ss;
                    dumpnode(ss, doc, node);
                    return ss.str();
                }
            }
        }
    }

    return {};
}

std::string search(std::string text, std::vector<std::string> terms) {
    std::stringstream ss;
    ss << text;
    std::istream& is = ss;

    streamreader bmlreader(is);
    byuuML::document doc(bmlreader);

    for (auto&& node : doc) {
        byuuML::cursor c = node.query(doc, terms[0]);
        if (c) {
            for (int i = 1; i < terms.size(); ++i) {
                c = c.query(terms[i]);
            }
            if (c) {
                std::string str(c.value<std::string>());
                str.erase(0, str.find_first_not_of(' '));
                return str;
            }
        }
    }

    return {};
}

std::vector<std::string> searchList(std::string text, std::vector<std::string> terms) {
    std::stringstream ss;
    ss << text;
    std::istream& is = ss;
    std::vector<std::string> ret;

    streamreader bmlreader(is);
    byuuML::document doc(bmlreader);

    for (auto&& node : doc) {
        byuuML::cursor c = node.query(doc, terms[0]);
        if (c) {
            for(auto&& cnode : byuuML::node_in_document(node, doc)) {
                for(auto&& ccnode : byuuML::node_in_document(cnode, doc)) {
                    if (cnode.name == terms[1]) {
                        byuuML::cursor cc = cnode.query(doc, terms[1]);
                        if (cc) {
                            if (ccnode.name == terms[2]) {
                                std::stringstream out;
                                dumpnode(out, doc, ccnode);
                                ret.push_back(out.str());
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

}

