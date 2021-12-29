#include "../sfc.hpp"

namespace SuperFamicom {

std::vector<uint8_t> NECDSP::firmware() const {
  std::vector<uint8_t> buffer;
  if(!cartridge.has.NECDSP) return buffer;
  unsigned plength = 2048, dlength = 1024;
  if(revision == Revision::uPD96050) plength = 16384, dlength = 2048;
  buffer.reserve(plength * 3 + dlength * 2);

  for(auto n : nall::range(plength)) {
    buffer.push_back(programROM[n] >>  0);
    buffer.push_back(programROM[n] >>  8);
    buffer.push_back(programROM[n] >> 16);
  }

  for(auto n : nall::range(dlength)) {
    buffer.push_back(dataROM[n] >> 0);
    buffer.push_back(dataROM[n] >> 8);
  }

  return buffer;
}

void NECDSP::serialize(serializer& s) {
  uPD96050::serialize(s);
  Thread::serialize(s);
}

NECDSP necdsp;

void NECDSP::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

void NECDSP::Enter() {
  while(true) {
    scheduler.synchronize();
    necdsp.main();
  }
}

void NECDSP::main() {
  exec();
  step(1);
  synchronizeCPU();
}

void NECDSP::step(unsigned clocks) {
  clock += clocks * (uint64_t)cpu.frequency;
}

uint8_t NECDSP::read(unsigned addr, uint8_t) {
  cpu.synchronizeCoprocessors();
  if(addr & 1) {
    return uPD96050::readSR();
  } else {
    return uPD96050::readDR();
  }
}

void NECDSP::write(unsigned addr, uint8_t data) {
  cpu.synchronizeCoprocessors();
  if(addr & 1) {
    return uPD96050::writeSR(data);
  } else {
    return uPD96050::writeDR(data);
  }
}

uint8_t NECDSP::readRAM(unsigned addr, uint8_t) {
  cpu.synchronizeCoprocessors();
  return uPD96050::readDP(addr);
}

void NECDSP::writeRAM(unsigned addr, uint8_t data) {
  cpu.synchronizeCoprocessors();
  return uPD96050::writeDP(addr, data);
}

void NECDSP::power() {
  uPD96050::power();
  create(NECDSP::Enter, Frequency);
}

}
