#pragma once

#include <unistd.h>

namespace nall {

//on Windows (at least for 7 and earlier), FILE* is not buffered
//thus, reading/writing one byte at a time will be dramatically slower
//on all other OSes, FILE* is buffered
//in order to ensure good performance, file_buffer implements its own buffer
//this speeds up Windows substantially, without harming performance elsewhere much

struct file_buffer {
  struct mode { enum : unsigned { read, write, modify, append }; };
  struct index { enum : unsigned { absolute, relative }; };

  file_buffer(const file_buffer&) = delete;
  auto operator=(const file_buffer&) -> file_buffer& = delete;

  file_buffer() = default;
  file_buffer(const string& filename, unsigned mode) { open(filename, mode); }

  ~file_buffer() { close(); }

  explicit operator bool() const {
    return (bool)fileHandle;
  }

  auto read() -> uint8_t {
    if(!fileHandle) return 0;              //file not open
    if(fileMode == mode::write) return 0;  //reads not permitted
    if(fileOffset >= fileSize) return 0;   //cannot read past end of file
    bufferSynchronize();
    return buffer[fileOffset++ & buffer.size() - 1];
  }

  auto read(array_span<uint8_t> memory) -> void {
    for(auto& byte : memory) byte = read();
  }

  auto write(uint8_t data) -> void {
    if(!fileHandle) return;             //file not open
    if(fileMode == mode::read) return;  //writes not permitted
    bufferSynchronize();
    buffer[fileOffset++ & buffer.size() - 1] = data;
    bufferDirty = true;
    if(fileOffset > fileSize) fileSize = fileOffset;
  }

  auto seek(int64_t offset, unsigned index_ = index::absolute) -> void {
    if(!fileHandle) return;
    bufferFlush();

    int64_t seekOffset = fileOffset;
    switch(index_) {
    case index::absolute: seekOffset  = offset; break;
    case index::relative: seekOffset += offset; break;
    }

    if(seekOffset < 0) seekOffset = 0;  //cannot seek before start of file
    if(seekOffset > fileSize) {
      if(fileMode == mode::read) {      //cannot seek past end of file
        seekOffset = fileSize;
      } else {                          //pad file to requested location
        fileOffset = fileSize;
        while(fileSize < seekOffset) write(0);
      }
    }

    fileOffset = seekOffset;
  }

  auto offset() const -> uint64_t {
    if(!fileHandle) return 0;
    return fileOffset;
  }

  auto size() const -> uint64_t {
    if(!fileHandle) return 0;
    return fileSize;
  }

  auto open(const string& filename, unsigned mode_) -> bool {
    close();

    switch(fileMode = mode_) {
    #if defined(_WIN32)
    case mode::read:   fileHandle = _wfopen(utf16_t(filename), L"rb" ); break;
    case mode::write:  fileHandle = _wfopen(utf16_t(filename), L"wb+"); break;
    case mode::modify: fileHandle = _wfopen(utf16_t(filename), L"rb+"); break;
    case mode::append: fileHandle = _wfopen(utf16_t(filename), L"wb+"); break;
    #else
    case mode::read:   fileHandle = fopen(filename, "rb" ); break;
    case mode::write:  fileHandle = fopen(filename, "wb+"); break;  //need read permission for buffering
    case mode::modify: fileHandle = fopen(filename, "rb+"); break;
    case mode::append: fileHandle = fopen(filename, "wb+"); break;
    #endif
    }
    if(!fileHandle) return false;

    bufferOffset = -1;
    fileOffset = 0;
    fseek(fileHandle, 0, SEEK_END);
    fileSize = ftell(fileHandle);
    fseek(fileHandle, 0, SEEK_SET);
    return true;
  }

  auto close() -> void {
    if(!fileHandle) return;
    bufferFlush();
    fclose(fileHandle);
    fileHandle = nullptr;
  }

private:
  array<uint8_t[4096]> buffer;
  int bufferOffset = -1;
  bool bufferDirty = false;
  FILE* fileHandle = nullptr;
  uint64_t fileOffset = 0;
  uint64_t fileSize = 0;
  unsigned fileMode = mode::read;

  auto bufferSynchronize() -> void {
    if(!fileHandle) return;
    if(bufferOffset == (fileOffset & ~(buffer.size() - 1))) return;

    bufferFlush();
    bufferOffset = fileOffset & ~(buffer.size() - 1);
    fseek(fileHandle, bufferOffset, SEEK_SET);
    uint64_t length = bufferOffset + buffer.size() <= fileSize ? buffer.size() : fileSize & buffer.size() - 1;
    if(length) (void)fread(buffer.data(), 1, length, fileHandle);
  }

  auto bufferFlush() -> void {
    if(!fileHandle) return;             //file not open
    if(fileMode == mode::read) return;  //buffer cannot be written to
    if(bufferOffset < 0) return;        //buffer unused
    if(!bufferDirty) return;            //buffer unmodified since read

    fseek(fileHandle, bufferOffset, SEEK_SET);
    uint64_t length = bufferOffset + buffer.size() <= fileSize ? buffer.size() : fileSize & buffer.size() - 1;
    if(length) (void)fwrite(buffer.data(), 1, length, fileHandle);
    bufferOffset = -1;
    bufferDirty = false;
  }
};

}
