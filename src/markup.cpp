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
    out << node.get_name();
    if (!node.get_data().empty()) out << ":" << node.get_data();
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
                    if (node.get_name() != parent) continue;
                    std::stringstream ss;
                    dumpnode(ss, doc, node);
                    return ss.str();
                }
            }
        }
    }

    return {};
}

std::string boardsearch(std::string text, std::string board) {
    std::stringstream ss;
    ss << text;
    std::istream& is = ss;

    streamreader bmlreader(is);
    byuuML::document doc(bmlreader);

    for (auto&& node : doc) {
        byuuML::cursor c = node.query(doc, "board");
        if (c) {
            std::string b = c.value<std::string>();
            b.erase(0, b.find_first_not_of(' '));
            if (b == board) {
                std::stringstream out;
                dumpnode(out, doc, node);
                return out.str();
            }
            else if (b.find("(") != std::string::npos) {
                std::string v = b;
                v.erase(0, v.find_first_of("(") + 1);
                v.erase(v.find_first_of(")"), v.length());
                b.erase(b.find_last_of('-') + 1, b.length());

                std::vector<std::string> ver;
                std::stringstream ss_ver(v);

                while(std::getline(ss_ver, v, ',')) {
                    ver.push_back(v);
                }

                for (std::string& version : ver) {
                    std::string boardversion = b + version;
                    if (boardversion == board) {
                        std::stringstream out;
                        dumpnode(out, doc, node);
                        return out.str();
                    }
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

std::string searchnode(std::string text, std::vector<std::string> terms) {
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
                std::stringstream out;
                dumpnode(out, doc, c.get_node());
                return out.str();
            }
        }
    }

    return {};
}

void traverse(std::vector<std::string>& ret, const byuuML::document& doc, const byuuML::node& node, std::string& term) {
    for (auto&& child : byuuML::node_in_document(node, doc)) {
        if (child.get_name() == term) {
            std::stringstream ss;
            dumpnode(ss, doc, child);
            ret.push_back(ss.str());
        }
        traverse(ret, doc, child, term);
    }
}

std::vector<std::string> searchList(std::string text, std::string term) {
    std::stringstream ss;
    ss << text;
    std::istream& is = ss;
    std::vector<std::string> ret;

    streamreader bmlreader(is);
    byuuML::document doc(bmlreader);

    for (auto&& node : doc) {
        traverse(ret, doc, node, term);
    }

    return ret;
}

std::vector<std::string> searchListShallow(std::string text, std::string parent, std::string child) {
    std::stringstream ss;
    ss << text;
    std::istream& is = ss;
    std::vector<std::string> ret;

    streamreader bmlreader(is);
    byuuML::document doc(bmlreader);

    for(auto&& c : doc[parent][child]) {
        std::stringstream out;
        dumpnode(out, doc, c.get_node());
        ret.push_back(out.str());
    }

    return ret;
}

bool exists(std::string text, std::vector<std::string> terms) {
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
                return true;
            }
        }
    }

    return false;
}

}
