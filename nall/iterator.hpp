/*
 * nall - C++ template library
 *
 * Copyright © 2006-2020 byuu et al
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

namespace nall {

struct range_t {
  struct iterator {
    iterator(int64_t position, int64_t step = 0) : position(position), step(step) {}
    auto operator*() const -> int64_t { return position; }
    auto operator!=(const iterator& source) const -> bool { return step > 0 ? position < source.position : position > source.position; }
    auto operator++() -> iterator& { position += step; return *this; }

  private:
    int64_t position;
    const int64_t step;
  };

  auto begin() const -> iterator { return {origin, stride}; }
  auto end() const -> iterator { return {target}; }

  int64_t origin;
  int64_t target;
  int64_t stride;
};

inline auto range(int64_t size) {
  return range_t{0, size, 1};
}

inline auto range(int64_t offset, int64_t size) {
  return range_t{offset, size, 1};
}

}
