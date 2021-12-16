#pragma once

namespace nall {

struct file : inode {
  struct mode { enum : uint { read, write, modify, append }; };

  static auto open(const string& filename, uint mode) -> file_buffer {
    return file_buffer{filename, mode};
  }

  static auto read(const string& filename) -> vector<uint8_t> {
    vector<uint8_t> memory;
    if(auto fp = file::open(filename, mode::read)) {
      memory.resize(fp.size());
      fp.read(memory);
    }
    return memory;
  }
};

}
