#pragma once

namespace BML {
std::string gendoc(std::string dbpath, std::string parent, std::string child, std::string match);
std::string boardsearch(std::string text, std::string board);
std::string search(std::string text, std::vector<std::string> terms);
std::string searchnode(std::string text, std::vector<std::string> terms);
std::vector<std::string> searchList(std::string text, std::string term);
std::vector<std::string> searchListShallow(std::string text, std::string parent, std::string child);
bool exists(std::string text, std::vector<std::string> terms);
}
