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

#pragma once

namespace BML {

std::string gendoc(std::string dbpath, std::string parent, std::string child, std::string match);
bool exists(std::string text, std::vector<std::string> terms);
std::string search(std::string text, std::vector<std::string> terms);
std::string searchBoard(std::string text, std::string board);
std::vector<std::string> searchList(std::string text, std::string term);
std::vector<std::string> searchListShallow(std::string text, std::string parent, std::string child);
std::string searchNode(std::string text, std::vector<std::string> terms);

}
