#include <sfc/sfc.hpp>

namespace SuperFamicom {

auto NECDSP::firmware() const -> vector<uint8_t> {
  vector<uint8_t> buffer;
  if(!cartridge.has.NECDSP) return buffer;
  unsigned plength = 2048, dlength = 1024;
  if(revision == Revision::uPD96050) plength = 16384, dlength = 2048;
  buffer.reserve(plength * 3 + dlength * 2);

  for(auto n : range(plength)) {
    buffer.append(programROM[n] >>  0);
    buffer.append(programROM[n] >>  8);
    buffer.append(programROM[n] >> 16);
  }

  for(auto n : range(dlength)) {
    buffer.append(dataROM[n] >> 0);
    buffer.append(dataROM[n] >> 8);
  }

  return buffer;
}

auto NECDSP::serialize(serializer& s) -> void {
  uPD96050::serialize(s);
  Thread::serialize(s);
}

NECDSP necdsp;

auto NECDSP::synchronizeCPU() -> void {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

auto NECDSP::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    necdsp.main();
  }
}

auto NECDSP::main() -> void {
  exec();
  step(1);
  synchronizeCPU();
}

auto NECDSP::step(unsigned clocks) -> void {
  clock += clocks * (uint64_t)cpu.frequency;
}

auto NECDSP::read(unsigned addr, uint8_t) -> uint8_t {
  cpu.synchronizeCoprocessors();
  if(addr & 1) {
    return uPD96050::readSR();
  } else {
    return uPD96050::readDR();
  }
}

auto NECDSP::write(unsigned addr, uint8_t data) -> void {
  cpu.synchronizeCoprocessors();
  if(addr & 1) {
    return uPD96050::writeSR(data);
  } else {
    return uPD96050::writeDR(data);
  }
}

auto NECDSP::readRAM(unsigned addr, uint8_t) -> uint8_t {
  cpu.synchronizeCoprocessors();
  return uPD96050::readDP(addr);
}

auto NECDSP::writeRAM(unsigned addr, uint8_t data) -> void {
  cpu.synchronizeCoprocessors();
  return uPD96050::writeDP(addr, data);
}

auto NECDSP::power() -> void {
  uPD96050::power();
  create(NECDSP::Enter, Frequency);
}

}
