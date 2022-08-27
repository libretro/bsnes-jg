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

std::string gendoc(std::istream&, std::string, std::string, std::string);
bool exists(std::string, std::vector<std::string>);
std::string search(std::string, std::vector<std::string>);
std::string searchBoard(std::string, std::string);
std::vector<std::string> searchList(std::string, std::string);
std::vector<std::string> searchListShallow(std::string, std::string, std::string);
std::string searchNode(std::string, std::vector<std::string>);

}
