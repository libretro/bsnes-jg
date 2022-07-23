/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2020-2022 Rupert Carmichael
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, specifically version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "byuuML/byuuML.hpp"
#include "byuuML/byuuML_query.hpp"

#include "markup.hpp"

namespace BML {

class streamreader : public byuuML::reader {
    char buf[512];
    std::istream& is;
public:
    streamreader(std::istream& _is) : is(_is) {}
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

// He bought? Dump eet.
static void dumpnode(std::ostream& out, const byuuML::document& document, const byuuML::node& node, int indent_level = 0) {
    for (int n = 0; n < indent_level; ++n) out << "  ";
    out << node.get_name();
    if (!node.get_data().empty()) out << ":" << node.get_data();
    out << "\n";

    for (auto&& child : byuuML::node_in_document(node, document)) {
        dumpnode(out, document, child, indent_level + 1);
    }
}

std::string gendoc(std::istream& is, std::string parent, std::string child, std::string val) {
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

std::string searchBoard(std::string text, std::string board) {
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
            for (size_t i = 1; i < terms.size(); ++i) {
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

std::string searchNode(std::string text, std::vector<std::string> terms) {
    std::stringstream ss;
    ss << text;
    std::istream& is = ss;

    streamreader bmlreader(is);
    byuuML::document doc(bmlreader);

    for (auto&& node : doc) {
        byuuML::cursor c = node.query(doc, terms[0]);
        if (c) {
            for (size_t i = 1; i < terms.size(); ++i) {
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

static void traverse(std::vector<std::string>& ret, const byuuML::document& doc, const byuuML::node& node, std::string& term) {
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
            for (size_t i = 1; i < terms.size(); ++i) {
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
