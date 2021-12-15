#pragma once

#include <sys/stat.h>

namespace nall {

struct file : inode {
  struct mode { enum : uint { read, write, modify, append }; };
  struct index { enum : uint { absolute, relative }; };

  file() = delete;

  static auto open(const string& filename, uint mode) -> file_buffer {
    return file_buffer{filename, mode};
  }

  static auto copy(const string& sourcename, const string& targetname) -> bool {
    if(sourcename == targetname) return true;
    if(auto reader = file::open(sourcename, mode::read)) {
      if(auto writer = file::open(targetname, mode::write)) {
        for(uint64_t n : range(reader.size())) writer.write(reader.read());
        return true;
      }
    }
    return false;
  }

  static auto truncate(const string& filename, uint64_t size) -> bool {
    #if defined(_WIN32)
    if(auto fp = _wfopen(utf16_t(filename), L"rb+")) {
      bool result = _chsize(fileno(fp), size) == 0;
      fclose(fp);
      return result;
    }
    return false;
    #else
    return truncate(filename, size) == 0;
    #endif
  }

  //returns false if specified filename is a directory
  static auto exists(const string& filename) -> bool {
    #if defined(_WIN32)
    struct __stat64 data;
    if(_wstat64(utf16_t(filename), &data) != 0) return false;
    #else
    struct stat data;
    if(stat(filename, &data) != 0) return false;
    #endif
    return !(data.st_mode & S_IFDIR);
  }

  static auto size(const string& filename) -> uint64_t {
    #if defined(_WIN32)
    struct __stat64 data;
    _wstat64(utf16_t(filename), &data);
    #else
    struct stat data;
    stat(filename, &data);
    #endif
    return S_ISREG(data.st_mode) ? data.st_size : 0u;
  }

  static auto read(const string& filename) -> vector<uint8_t> {
    vector<uint8_t> memory;
    if(auto fp = file::open(filename, mode::read)) {
      memory.resize(fp.size());
      fp.read(memory);
    }
    return memory;
  }

  static auto read(const string& filename, array_span<uint8_t> memory) -> bool {
    if(auto fp = file::open(filename, mode::read)) return fp.read(memory), true;
    return false;
  }

  static auto write(const string& filename, array_view<uint8_t> memory) -> bool {
    if(auto fp = file::open(filename, mode::write)) return fp.write(memory), true;
    return false;
  }

  //create an empty file (will replace existing files)
  static auto create(const string& filename) -> bool {
    if(auto fp = file::open(filename, mode::write)) return true;
    return false;
  }

  static auto sha256(const string& filename) -> string {
    return Hash::SHA256(read(filename)).digest();
  }
};

}
