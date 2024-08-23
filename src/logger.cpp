/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2020-2024 Rupert Carmichael
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

#include <iostream>

#include "logger.hpp"

Logger logger;

static void log_default(void*, int, std::string& text) {
    std::cerr << text;
}

void Logger::log(int level, std::string text) {
    log_cb(udata, level, text);
}

Logger::Logger() {
    setCallback(nullptr, &log_default);
}

void Logger::setCallback(void *ptr, void (*cb)(void*, int, std::string&)) {
    log_cb = cb;
    udata = ptr;
}
