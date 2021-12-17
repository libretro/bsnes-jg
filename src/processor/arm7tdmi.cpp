#include <emulator/emulator.hpp>
#include "arm7tdmi.hpp"

namespace Processor {

auto ARM7TDMI::r(nall::Natural< 4> index) -> GPR& {
  switch(index) {
  case  0: return processor.r0;
  case  1: return processor.r1;
  case  2: return processor.r2;
  case  3: return processor.r3;
  case  4: return processor.r4;
  case  5: return processor.r5;
  case  6: return processor.r6;
  case  7: return processor.r7;
  case  8: return processor.cpsr.m == PSR::FIQ ? processor.fiq.r8  : processor.r8;
  case  9: return processor.cpsr.m == PSR::FIQ ? processor.fiq.r9  : processor.r9;
  case 10: return processor.cpsr.m == PSR::FIQ ? processor.fiq.r10 : processor.r10;
  case 11: return processor.cpsr.m == PSR::FIQ ? processor.fiq.r11 : processor.r11;
  case 12: return processor.cpsr.m == PSR::FIQ ? processor.fiq.r12 : processor.r12;
  case 13: switch(processor.cpsr.m) {
    case PSR::FIQ: return processor.fiq.r13;
    case PSR::IRQ: return processor.irq.r13;
    case PSR::SVC: return processor.svc.r13;
    case PSR::ABT: return processor.abt.r13;
    case PSR::UND: return processor.und.r13;
    default: return processor.r13;
  }
  case 14: switch(processor.cpsr.m) {
    case PSR::FIQ: return processor.fiq.r14;
    case PSR::IRQ: return processor.irq.r14;
    case PSR::SVC: return processor.svc.r14;
    case PSR::ABT: return processor.abt.r14;
    case PSR::UND: return processor.und.r14;
    default: return processor.r14;
  }
  case 15: return processor.r15;
  }
  throw; // unreachable
}

auto ARM7TDMI::cpsr() -> PSR& {
  return processor.cpsr;
}

auto ARM7TDMI::spsr() -> PSR& {
  switch(processor.cpsr.m) {
  case PSR::FIQ: return processor.fiq.spsr;
  case PSR::IRQ: return processor.irq.spsr;
  case PSR::SVC: return processor.svc.spsr;
  case PSR::ABT: return processor.abt.spsr;
  case PSR::UND: return processor.und.spsr;
  }
  throw;
}

auto ARM7TDMI::privileged() const -> bool {
  return processor.cpsr.m != PSR::USR;
}

auto ARM7TDMI::exception() const -> bool {
  return privileged() && processor.cpsr.m != PSR::SYS;
}

auto ARM7TDMI::idle() -> void {
  pipeline.nonsequential = true;
  sleep();
}

auto ARM7TDMI::read(unsigned mode, uint32_t address) -> uint32_t {
  return get(mode, address);
}

auto ARM7TDMI::load(unsigned mode, uint32_t address) -> uint32_t {
  pipeline.nonsequential = true;
  auto word = get(Load | mode, address);
  if(mode & Half) {
    address &= 1;
    word = mode & Signed ? (uint32_t)(int16_t)word : (uint32_t)(uint16_t)word;
  }
  if(mode & Byte) {
    address &= 0;
    word = mode & Signed ? (uint32_t)(int8_t)word : (uint32_t)(uint8_t)word;
  }
  if(mode & Signed) {
    word = ASR(word, (address & 3) << 3);
  } else {
    word = ROR(word, (address & 3) << 3);
  }
  idle();
  return word;
}

auto ARM7TDMI::write(unsigned mode, uint32_t address, uint32_t word) -> void {
  pipeline.nonsequential = true;
  return set(mode, address, word);
}

auto ARM7TDMI::store(unsigned mode, uint32_t address, uint32_t word) -> void {
  pipeline.nonsequential = true;
  if(mode & Half) { word &= 0xffff; word |= word << 16; }
  if(mode & Byte) { word &= 0xff; word |= word << 8; word |= word << 16; }
  return set(Store | mode, address, word);
}

auto ARM7TDMI::ADD(uint32_t source, uint32_t modify, bool carry) -> uint32_t {
  uint32_t result = source + modify + carry;
  if(cpsr().t || (opcode & 1 << 20)) {
    uint32_t overflow = ~(source ^ modify) & (source ^ result);
    cpsr().v = 1 << 31 & (overflow);
    cpsr().c = 1 << 31 & (overflow ^ source ^ modify ^ result);
    cpsr().z = result == 0;
    cpsr().n = result >> 31;
  }
  return result;
}

auto ARM7TDMI::ASR(uint32_t source, uint8_t shift) -> uint32_t {
  carry = cpsr().c;
  if(shift == 0) return source;
  carry = shift > 32 ? source & 1 << 31 : source & 1 << shift - 1;
  source = shift > 31 ? (int32_t)source >> 31 : (int32_t)source >> shift;
  return source;
}

auto ARM7TDMI::BIT(uint32_t result) -> uint32_t {
  if(cpsr().t || (opcode & 1 << 20)) {
    cpsr().c = carry;
    cpsr().z = result == 0;
    cpsr().n = result >> 31;
  }
  return result;
}

auto ARM7TDMI::LSL(uint32_t source, uint8_t shift) -> uint32_t {
  carry = cpsr().c;
  if(shift == 0) return source;
  carry = shift > 32 ? 0 : source & 1 << 32 - shift;
  source = shift > 31 ? 0 : source << shift;
  return source;
}

auto ARM7TDMI::LSR(uint32_t source, uint8_t shift) -> uint32_t {
  carry = cpsr().c;
  if(shift == 0) return source;
  carry = shift > 32 ? 0 : source & 1 << shift - 1;
  source = shift > 31 ? 0 : source >> shift;
  return source;
}

auto ARM7TDMI::MUL(uint32_t product, uint32_t multiplicand, uint32_t multiplier) -> uint32_t {
  idle();
  if(multiplier >>  8 && multiplier >>  8 != 0xffffff) idle();
  if(multiplier >> 16 && multiplier >> 16 !=   0xffff) idle();
  if(multiplier >> 24 && multiplier >> 24 !=     0xff) idle();
  product += multiplicand * multiplier;
  if(cpsr().t || (opcode & 1 << 20)) {
    cpsr().z = product == 0;
    cpsr().n = product >> 31;
  }
  return product;
}

auto ARM7TDMI::ROR(uint32_t source, uint8_t shift) -> uint32_t {
  carry = cpsr().c;
  if(shift == 0) return source;
  if(shift &= 31) source = source << 32 - shift | source >> shift;
  carry = source & 1 << 31;
  return source;
}

auto ARM7TDMI::RRX(uint32_t source) -> uint32_t {
  carry = source & 1;
  return cpsr().c << 31 | source >> 1;
}

auto ARM7TDMI::SUB(uint32_t source, uint32_t modify, bool carry) -> uint32_t {
  return ADD(source, ~modify, carry);
}

auto ARM7TDMI::TST(nall::Natural< 4> mode) -> bool {
  switch(mode) {
  case  0: return cpsr().z == 1;                          //EQ (equal)
  case  1: return cpsr().z == 0;                          //NE (not equal)
  case  2: return cpsr().c == 1;                          //CS (carry set)
  case  3: return cpsr().c == 0;                          //CC (carry clear)
  case  4: return cpsr().n == 1;                          //MI (negative)
  case  5: return cpsr().n == 0;                          //PL (positive)
  case  6: return cpsr().v == 1;                          //VS (overflow)
  case  7: return cpsr().v == 0;                          //VC (no overflow)
  case  8: return cpsr().c == 1 && cpsr().z == 0;         //HI (unsigned higher)
  case  9: return cpsr().c == 0 || cpsr().z == 1;         //LS (unsigned lower or same)
  case 10: return cpsr().n == cpsr().v;                   //GE (signed greater than or equal)
  case 11: return cpsr().n != cpsr().v;                   //LT (signed less than)
  case 12: return cpsr().z == 0 && cpsr().n == cpsr().v;  //GT (signed greater than)
  case 13: return cpsr().z == 1 || cpsr().n != cpsr().v;  //LE (signed less than or equal)
  case 14: return true;                                   //AL (always)
  case 15: return false;                                  //NV (never)
  }
  return 0; // unreachable
}

auto ARM7TDMI::fetch() -> void {
  pipeline.execute = pipeline.decode;
  pipeline.decode = pipeline.fetch;
  pipeline.decode.thumb = cpsr().t;

  unsigned sequential = Sequential;
  if(pipeline.nonsequential) {
    pipeline.nonsequential = false;
    sequential = Nonsequential;
  }

  unsigned mask = !cpsr().t ? 3 : 1;
  unsigned size = !cpsr().t ? Word : Half;

  r(15).data += size >> 3;
  pipeline.fetch.address = r(15) & ~mask;
  pipeline.fetch.instruction = read(Prefetch | size | sequential, pipeline.fetch.address);
}

auto ARM7TDMI::instruction() -> void {
  unsigned mask = !cpsr().t ? 3 : 1;
  unsigned size = !cpsr().t ? Word : Half;

  if(pipeline.reload) {
    pipeline.reload = false;
    r(15).data &= ~mask;
    pipeline.fetch.address = r(15) & ~mask;
    pipeline.fetch.instruction = read(Prefetch | size | Nonsequential, pipeline.fetch.address);
    fetch();
  }
  fetch();

  if(irq && !cpsr().i) {
    exception(PSR::IRQ, 0x18);
    if(pipeline.execute.thumb) r(14).data += 2;
    return;
  }

  opcode = pipeline.execute.instruction;
  if(!pipeline.execute.thumb) {
    if(!TST(opcode >> 28)) return;
    nall::Natural<12> index = (opcode & 0x0ff00000) >> 16 | (opcode & 0x000000f0) >> 4;
    armInstruction[index](opcode);
  } else {
    thumbInstruction[(uint16_t)opcode]();
  }
}

auto ARM7TDMI::exception(unsigned mode, uint32_t address) -> void {
  auto psr = cpsr();
  cpsr().m = mode;
  spsr() = psr;
  cpsr().t = 0;
  if(cpsr().m == PSR::FIQ) cpsr().f = 1;
  cpsr().i = 1;
  r(14) = pipeline.decode.address;
  r(15) = address;
}

auto ARM7TDMI::armInitialize() -> void {
  #define bind(id, name, ...) { \
    unsigned index = (id & 0x0ff00000) >> 16 | (id & 0x000000f0) >> 4; \
    assert(!armInstruction[index]); \
    armInstruction[index] = [&](uint32_t opcode) { return armInstruction##name(arguments); }; \
    armDisassemble[index] = [&](uint32_t opcode) { return armDisassemble##name(arguments); }; \
  }

  #define pattern(s) \
    std::integral_constant<uint32_t, nall::test(s)>::value

  #define bit1(value, index) (value >> index & 1)
  #define bits(value, lo, hi) (value >> lo & (1ull << (hi - lo + 1)) - 1)

  #define arguments \
    bits(opcode, 0,23),  /* displacement */ \
    bit1(opcode,24)      /* link */
  for(nall::Natural< 4> displacementLo : range(16))
  for(nall::Natural< 4> displacementHi : range(16))
  for(nall::Natural< 1> link : range(2)) {
    auto opcode = pattern(".... 101? ???? ???? ???? ???? ???? ????")
                | displacementLo << 4 | displacementHi << 20 | link << 24;
    bind(opcode, Branch);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3)   /* m */
  {
    auto opcode = pattern(".... 0001 0010 ---- ---- ---- 0001 ????");
    bind(opcode, BranchExchangeRegister);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 7),  /* immediate */ \
    bits(opcode, 8,11),  /* shift */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* save */ \
    bits(opcode,21,24)   /* mode */
  for(nall::Natural< 4> shiftHi : range(16))
  for(nall::Natural< 1> save : range(2))
  for(nall::Natural< 4> mode : range(16)) {
    if(mode >= 8 && mode <= 11 && !save) continue;  //TST, TEQ, CMP, CMN
    auto opcode = pattern(".... 001? ???? ???? ???? ???? ???? ????") | shiftHi << 4 | save << 20 | mode << 21;
    bind(opcode, DataImmediate);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode, 5, 6),  /* type */ \
    bits(opcode, 7,11),  /* shift */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* save */ \
    bits(opcode,21,24)   /* mode */
  for(nall::Natural< 2> type : range(4))
  for(nall::Natural< 1> shiftLo : range(2))
  for(nall::Natural< 1> save : range(2))
  for(nall::Natural< 4> mode : range(16)) {
    if(mode >= 8 && mode <= 11 && !save) continue;  //TST, TEQ, CMP, CMN
    auto opcode = pattern(".... 000? ???? ???? ???? ???? ???0 ????") | type << 5 | shiftLo << 7 | save << 20 | mode << 21;
    bind(opcode, DataImmediateShift);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode, 5, 6),  /* type */ \
    bits(opcode, 8,11),  /* s */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* save */ \
    bits(opcode,21,24)   /* mode */
  for(nall::Natural< 2> type : range(4))
  for(nall::Natural< 1> save : range(2))
  for(nall::Natural< 4> mode : range(16)) {
    if(mode >= 8 && mode <= 11 && !save) continue;  //TST, TEQ, CMP, CMN
    auto opcode = pattern(".... 000? ???? ???? ???? ???? 0??1 ????") | type << 5 | save << 20 | mode << 21;
    bind(opcode, DataRegisterShift);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3) << 0 | bits(opcode, 8,11) << 4,  /* immediate */ \
    bit1(opcode, 5),     /* half */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(nall::Natural< 1> half : range(2))
  for(nall::Natural< 1> writeback : range(2))
  for(nall::Natural< 1> up : range(2))
  for(nall::Natural< 1> pre : range(2)) {
    auto opcode = pattern(".... 000? ?1?1 ???? ???? ???? 11?1 ????") | half << 5 | writeback << 21 | up << 23 | pre << 24;
    bind(opcode, LoadImmediate);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bit1(opcode, 5),     /* half */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(nall::Natural< 1> half : range(2))
  for(nall::Natural< 1> writeback : range(2))
  for(nall::Natural< 1> up : range(2))
  for(nall::Natural< 1> pre : range(2)) {
    auto opcode = pattern(".... 000? ?0?1 ???? ???? ---- 11?1 ????") | half << 5 | writeback << 21 | up << 23 | pre << 24;
    bind(opcode, LoadRegister);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,22)      /* byte */
  for(nall::Natural< 1> byte : range(2)) {
    auto opcode = pattern(".... 0001 0?00 ???? ???? ---- 1001 ????") | byte << 22;
    bind(opcode, MemorySwap);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3) << 0 | bits(opcode, 8,11) << 4,  /* immediate */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* mode */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(nall::Natural< 1> mode : range(2))
  for(nall::Natural< 1> writeback : range(2))
  for(nall::Natural< 1> up : range(2))
  for(nall::Natural< 1> pre : range(2)) {
    auto opcode = pattern(".... 000? ?1?? ???? ???? ???? 1011 ????") | mode << 20 | writeback << 21 | up << 23 | pre << 24;
    bind(opcode, MoveHalfImmediate);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* mode */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(nall::Natural< 1> mode : range(2))
  for(nall::Natural< 1> writeback : range(2))
  for(nall::Natural< 1> up : range(2))
  for(nall::Natural< 1> pre : range(2)) {
    auto opcode = pattern(".... 000? ?0?? ???? ???? ---- 1011 ????") | mode << 20 | writeback << 21 | up << 23 | pre << 24;
    bind(opcode, MoveHalfRegister);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0,11),  /* immediate */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* mode */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,22),     /* byte */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(nall::Natural< 4> immediatePart : range(16))
  for(nall::Natural< 1> mode : range(2))
  for(nall::Natural< 1> writeback : range(2))
  for(nall::Natural< 1> byte : range(2))
  for(nall::Natural< 1> up : range(2))
  for(nall::Natural< 1> pre : range(2)) {
    auto opcode = pattern(".... 010? ???? ???? ???? ???? ???? ????")
                | immediatePart << 4 | mode << 20 | writeback << 21 | byte << 22 | up << 23 | pre << 24;
    bind(opcode, MoveImmediateOffset);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0,15),  /* list */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* mode */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,22),     /* type */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(nall::Natural< 4> listPart : range(16))
  for(nall::Natural< 1> mode : range(2))
  for(nall::Natural< 1> writeback : range(2))
  for(nall::Natural< 1> type : range(2))
  for(nall::Natural< 1> up : range(2))
  for(nall::Natural< 1> pre : range(2)) {
    auto opcode = pattern(".... 100? ???? ???? ???? ???? ???? ????")
                | listPart << 4 | mode << 20 | writeback << 21 | type << 22 | up << 23 | pre << 24;
    bind(opcode, MoveMultiple);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode, 5, 6),  /* type */ \
    bits(opcode, 7,11),  /* shift */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* mode */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,22),     /* byte */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(nall::Natural< 2> type : range(4))
  for(nall::Natural< 1> shiftLo : range(2))
  for(nall::Natural< 1> mode : range(2))
  for(nall::Natural< 1> writeback : range(2))
  for(nall::Natural< 1> byte : range(2))
  for(nall::Natural< 1> up : range(2))
  for(nall::Natural< 1> pre : range(2)) {
    auto opcode = pattern(".... 011? ???? ???? ???? ???? ???0 ????")
                | type << 5 | shiftLo << 7 | mode << 20 | writeback << 21 | byte << 22 | up << 23 | pre << 24;
    bind(opcode, MoveRegisterOffset);
  }
  #undef arguments

  #define arguments \
    bits(opcode,12,15),  /* d */ \
    bit1(opcode,22)      /* mode */
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern(".... 0001 0?00 ---- ???? ---- 0000 ----") | mode << 22;
    bind(opcode, MoveToRegisterFromStatus);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 7),  /* immediate */ \
    bits(opcode, 8,11),  /* rotate */ \
    bits(opcode,16,19),  /* field */ \
    bit1(opcode,22)      /* mode */
  for(nall::Natural< 4> immediateHi : range(16))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern(".... 0011 0?10 ???? ---- ???? ???? ????") | immediateHi << 4 | mode << 22;
    bind(opcode, MoveToStatusFromImmediate);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode,16,19),  /* field */ \
    bit1(opcode,22)      /* mode */
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern(".... 0001 0?10 ???? ---- ---- 0000 ????") | mode << 22;
    bind(opcode, MoveToStatusFromRegister);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode, 8,11),  /* s */ \
    bits(opcode,12,15),  /* n */ \
    bits(opcode,16,19),  /* d */ \
    bit1(opcode,20),     /* save */ \
    bit1(opcode,21)      /* accumulate */
  for(nall::Natural< 1> save : range(2))
  for(nall::Natural< 1> accumulate : range(2)) {
    auto opcode = pattern(".... 0000 00?? ???? ???? ???? 1001 ????") | save << 20 | accumulate << 21;
    bind(opcode, Multiply);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode, 8,11),  /* s */ \
    bits(opcode,12,15),  /* l */ \
    bits(opcode,16,19),  /* h */ \
    bit1(opcode,20),     /* save */ \
    bit1(opcode,21),     /* accumulate */ \
    bit1(opcode,22)      /* sign */
  for(nall::Natural< 1> save : range(2))
  for(nall::Natural< 1> accumulate : range(2))
  for(nall::Natural< 1> sign : range(2)) {
    auto opcode = pattern(".... 0000 1??? ???? ???? ???? 1001 ????") | save << 20 | accumulate << 21 | sign << 22;
    bind(opcode, MultiplyLong);
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0,23)  /* immediate */
  for(nall::Natural< 4> immediateLo : range(16))
  for(nall::Natural< 4> immediateHi : range(16)) {
    auto opcode = pattern(".... 1111 ???? ???? ???? ???? ???? ????") | immediateLo << 4 | immediateHi << 20;
    bind(opcode, SoftwareInterrupt);
  }
  #undef arguments

  #define arguments
  for(nall::Natural<12> id : range(4096)) {
    if(armInstruction[id]) continue;
    auto opcode = pattern(".... ???? ???? ---- ---- ---- ???? ----") | bits(id,0,3) << 4 | bits(id,4,11) << 20;
    bind(opcode, Undefined);
  }
  #undef arguments

  #undef bind
  #undef pattern
}

auto ARM7TDMI::thumbInitialize() -> void {
  #define bind(id, name, ...) { \
    assert(!thumbInstruction[id]); \
    thumbInstruction[id] = [=] { return thumbInstruction##name(__VA_ARGS__); }; \
    thumbDisassemble[id] = [=] { return thumbDisassemble##name(__VA_ARGS__); }; \
  }

  #define pattern(s) \
    std::integral_constant<uint16_t, nall::test(s)>::value

  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 3> m : range(8))
  for(nall::Natural< 4> mode : range(16)) {
    auto opcode = pattern("0100 00?? ???? ????") | d << 0 | m << 3 | mode << 6;
    bind(opcode, ALU, d, m, mode);
  }

  for(nall::Natural< 4> d : range(16))
  for(nall::Natural< 4> m : range(16))
  for(nall::Natural< 2> mode : range(4)) {
    if(mode == 3) continue;
    auto opcode = pattern("0100 01?? ???? ????") | bits(d,0,2) << 0 | m << 3 | bit1(d,3) << 7 | mode << 8;
    bind(opcode, ALUExtended, d, m, mode);
  }

  for(uint8_t immediate : range(256))
  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern("1010 ???? ???? ????") | immediate << 0 | d << 8 | mode << 11;
    bind(opcode, AddRegister, immediate, d, mode);
  }

  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 3> n : range(8))
  for(nall::Natural< 3> immediate : range(8))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern("0001 11?? ???? ????") | d << 0 | n << 3 | immediate << 6 | mode << 9;
    bind(opcode, AdjustImmediate, d, n, immediate, mode);
  }

  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 3> n : range(8))
  for(nall::Natural< 3> m : range(8))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern("0001 10?? ???? ????") | d << 0 | n << 3 | m << 6 | mode << 9;
    bind(opcode, AdjustRegister, d, n, m, mode);
  }

  for(nall::Natural< 7> immediate : range(128))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern("1011 0000 ???? ????") | immediate << 0 | mode << 7;
    bind(opcode, AdjustStack, immediate, mode);
  }

  for(nall::Natural< 3> _ : range(8))
  for(nall::Natural< 4> m : range(16)) {
    auto opcode = pattern("0100 0111 0??? ?---") | _ << 0 | m << 3;
    bind(opcode, BranchExchange, m);
  }

  for(nall::Natural<11> displacement : range(2048)) {
    auto opcode = pattern("1111 0??? ???? ????") | displacement << 0;
    bind(opcode, BranchFarPrefix, displacement);
  }

  for(nall::Natural<11> displacement : range(2048)) {
    auto opcode = pattern("1111 1??? ???? ????") | displacement << 0;
    bind(opcode, BranchFarSuffix, displacement);
  }

  for(nall::Natural<11> displacement : range(2048)) {
    auto opcode = pattern("1110 0??? ???? ????") | displacement << 0;
    bind(opcode, BranchNear, displacement);
  }

  for(uint8_t displacement : range(256))
  for(nall::Natural< 4> condition : range(16)) {
    if(condition == 15) continue;  //BNV
    auto opcode = pattern("1101 ???? ???? ????") | displacement << 0 | condition << 8;
    bind(opcode, BranchTest, displacement, condition);
  }

  for(uint8_t immediate : range(256))
  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 2> mode : range(4)) {
    auto opcode = pattern("001? ???? ???? ????") | immediate << 0 | d << 8 | mode << 11;
    bind(opcode, Immediate, immediate, d, mode);
  }

  for(uint8_t displacement : range(256))
  for(nall::Natural< 3> d : range(8)) {
    auto opcode = pattern("0100 1??? ???? ????") | displacement << 0 | d << 8;
    bind(opcode, LoadLiteral, displacement, d);
  }

  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 3> n : range(8))
  for(nall::Natural< 5> immediate : range(32))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern("0111 ???? ???? ????") | d << 0 | n << 3 | immediate << 6 | mode << 11;
    bind(opcode, MoveByteImmediate, d, n, immediate, mode);
  }

  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 3> n : range(8))
  for(nall::Natural< 5> immediate : range(32))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern("1000 ???? ???? ????") | d << 0 | n << 3 | immediate << 6 | mode << 11;
    bind(opcode, MoveHalfImmediate, d, n, immediate, mode);
  }

  for(uint8_t list : range(256))
  for(nall::Natural< 3> n : range(8))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern("1100 ???? ???? ????") | list << 0 | n << 8 | mode << 11;
    bind(opcode, MoveMultiple, list, n, mode);
  }

  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 3> n : range(8))
  for(nall::Natural< 3> m : range(8))
  for(nall::Natural< 3> mode : range(8)) {
    auto opcode = pattern("0101 ???? ???? ????") | d << 0 | n << 3 | m << 6 | mode << 9;
    bind(opcode, MoveRegisterOffset, d, n, m, mode);
  }

  for(uint8_t immediate : range(256))
  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern("1001 ???? ???? ????") | immediate << 0 | d << 8 | mode << 11;
    bind(opcode, MoveStack, immediate, d, mode);
  }

  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 3> n : range(8))
  for(nall::Natural< 5> offset : range(32))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern("0110 ???? ???? ????") | d << 0 | n << 3 | offset << 6 | mode << 11;
    bind(opcode, MoveWordImmediate, d, n, offset, mode);
  }

  for(nall::Natural< 3> d : range(8))
  for(nall::Natural< 3> m : range(8))
  for(nall::Natural< 5> immediate : range(32))
  for(nall::Natural< 2> mode : range(4)) {
    if(mode == 3) continue;
    auto opcode = pattern("000? ???? ???? ????") | d << 0 | m << 3 | immediate << 6 | mode << 11;
    bind(opcode, ShiftImmediate, d, m, immediate, mode);
  }

  for(uint8_t immediate : range(256)) {
    auto opcode = pattern("1101 1111 ???? ????") | immediate << 0;
    bind(opcode, SoftwareInterrupt, immediate);
  }

  for(uint8_t list : range(256))
  for(nall::Natural< 1> lrpc : range(2))
  for(nall::Natural< 1> mode : range(2)) {
    auto opcode = pattern("1011 ?10? ???? ????") | list << 0 | lrpc << 8 | mode << 11;
    bind(opcode, StackMultiple, list, lrpc, mode);
  }

  for(uint16_t id : range(65536)) {
    if(thumbInstruction[id]) continue;
    auto opcode = pattern("???? ???? ???? ????") | id << 0;
    bind(opcode, Undefined);
  }

  #undef bit1
  #undef bits

  #undef bind
  #undef pattern
}

auto ARM7TDMI::armALU(nall::Natural< 4> mode, nall::Natural< 4> d, nall::Natural< 4> n, uint32_t rm) -> void {
  uint32_t rn = r(n);

  switch(mode) {
  case  0: r(d) = BIT(rn & rm); break;  //AND
  case  1: r(d) = BIT(rn ^ rm); break;  //EOR
  case  2: r(d) = SUB(rn, rm, 1); break;  //SUB
  case  3: r(d) = SUB(rm, rn, 1); break;  //RSB
  case  4: r(d) = ADD(rn, rm, 0); break;  //ADD
  case  5: r(d) = ADD(rn, rm, cpsr().c); break;  //ADC
  case  6: r(d) = SUB(rn, rm, cpsr().c); break;  //SBC
  case  7: r(d) = SUB(rm, rn, cpsr().c); break;  //RSC
  case  8:        BIT(rn & rm); break;  //TST
  case  9:        BIT(rn ^ rm); break;  //TEQ
  case 10:        SUB(rn, rm, 1); break;  //CMP
  case 11:        ADD(rn, rm, 0); break;  //CMN
  case 12: r(d) = BIT(rn | rm); break;  //ORR
  case 13: r(d) = BIT(rm); break;  //MOV
  case 14: r(d) = BIT(rn & ~rm); break;  //BIC
  case 15: r(d) = BIT(~rm); break;  //MVN
  }

  if(exception() && d == 15 && (opcode & 1 << 20)) {
    cpsr() = spsr();
  }
}

auto ARM7TDMI::armMoveToStatus(nall::Natural< 4> field, nall::Natural< 1> mode, uint32_t data) -> void {
  if(mode && (cpsr().m == PSR::USR || cpsr().m == PSR::SYS)) return;
  PSR& psr = mode ? spsr() : cpsr();

  if(field & 1) {
    if(mode || privileged()) {
      psr.m = data >> 0 & 31;
      psr.t = data >> 5 & 1;
      psr.f = data >> 6 & 1;
      psr.i = data >> 7 & 1;
      if(!mode && psr.t) r(15).data += 2;
    }
  }

  if(field & 8) {
    psr.v = data >> 28 & 1;
    psr.c = data >> 29 & 1;
    psr.z = data >> 30 & 1;
    psr.n = data >> 31 & 1;
  }
}

auto ARM7TDMI::armInstructionBranch
(nall::Integer<24> displacement, nall::Natural< 1> link) -> void {
  if(link) r(14) = r(15) - 4;
  r(15) = r(15) + displacement * 4;
}

auto ARM7TDMI::armInstructionBranchExchangeRegister
(nall::Natural< 4> m) -> void {
  uint32_t address = r(m);
  cpsr().t = address & 1;
  r(15) = address;
}

auto ARM7TDMI::armInstructionDataImmediate
(uint8_t immediate, nall::Natural< 4> shift, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> save, nall::Natural< 4> mode) -> void {
  uint32_t data = immediate;
  carry = cpsr().c;
  if(shift) data = ROR(data, shift << 1);
  armALU(mode, d, n, data);
}

auto ARM7TDMI::armInstructionDataImmediateShift
(nall::Natural< 4> m, nall::Natural< 2> type, nall::Natural< 5> shift, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> save, nall::Natural< 4> mode) -> void {
  uint32_t rm = r(m);
  carry = cpsr().c;

  switch(type) {
  case 0: rm = LSL(rm, shift); break;
  case 1: rm = LSR(rm, shift ? (unsigned)shift : 32); break;
  case 2: rm = ASR(rm, shift ? (unsigned)shift : 32); break;
  case 3: rm = shift ? ROR(rm, shift) : RRX(rm); break;
  }

  armALU(mode, d, n, rm);
}

auto ARM7TDMI::armInstructionDataRegisterShift
(nall::Natural< 4> m, nall::Natural< 2> type, nall::Natural< 4> s, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> save, nall::Natural< 4> mode) -> void {
  uint8_t rs = r(s) + (s == 15 ? 4 : 0);
  uint32_t rm = r(m) + (m == 15 ? 4 : 0);
  carry = cpsr().c;

  switch(type) {
  case 0: rm = LSL(rm, rs < 33 ? rs : (uint8_t)33); break;
  case 1: rm = LSR(rm, rs < 33 ? rs : (uint8_t)33); break;
  case 2: rm = ASR(rm, rs < 32 ? rs : (uint8_t)32); break;
  case 3: if(rs) rm = ROR(rm, rs & 31 ? unsigned(rs & 31) : 32); break;
  }

  armALU(mode, d, n, rm);
}

auto ARM7TDMI::armInstructionLoadImmediate
(uint8_t immediate, nall::Natural< 1> half, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> writeback, nall::Natural< 1> up, nall::Natural< 1> pre) -> void {
  uint32_t rn = r(n);
  uint32_t rd = r(d);

  if(pre == 1) rn = up ? rn + immediate : rn - immediate;
  rd = load((half ? Half : Byte) | Nonsequential | Signed, rn);
  if(pre == 0) rn = up ? rn + immediate : rn - immediate;

  if(pre == 0 || writeback) r(n) = rn;
  r(d) = rd;
}

auto ARM7TDMI::armInstructionLoadRegister
(nall::Natural< 4> m, nall::Natural< 1> half, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> writeback, nall::Natural< 1> up, nall::Natural< 1> pre) -> void {
  uint32_t rn = r(n);
  uint32_t rm = r(m);
  uint32_t rd = r(d);

  if(pre == 1) rn = up ? rn + rm : rn - rm;
  rd = load((half ? Half : Byte) | Nonsequential | Signed, rn);
  if(pre == 0) rn = up ? rn + rm : rn - rm;

  if(pre == 0 || writeback) r(n) = rn;
  r(d) = rd;
}

auto ARM7TDMI::armInstructionMemorySwap
(nall::Natural< 4> m, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> byte) -> void {
  uint32_t word = load((byte ? Byte : Word) | Nonsequential, r(n));
  store((byte ? Byte : Word) | Nonsequential, r(n), r(m));
  r(d) = word;
}

auto ARM7TDMI::armInstructionMoveHalfImmediate
(uint8_t immediate, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> mode, nall::Natural< 1> writeback, nall::Natural< 1> up, nall::Natural< 1> pre) -> void {
  uint32_t rn = r(n);
  uint32_t rd = r(d);

  if(pre == 1) rn = up ? rn + immediate : rn - immediate;
  if(mode == 1) rd = load(Half | Nonsequential, rn);
  if(mode == 0) store(Half | Nonsequential, rn, rd);
  if(pre == 0) rn = up ? rn + immediate : rn - immediate;

  if(pre == 0 || writeback) r(n) = rn;
  if(mode == 1) r(d) = rd;
}

auto ARM7TDMI::armInstructionMoveHalfRegister
(nall::Natural< 4> m, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> mode, nall::Natural< 1> writeback, nall::Natural< 1> up, nall::Natural< 1> pre) -> void {
  uint32_t rn = r(n);
  uint32_t rm = r(m);
  uint32_t rd = r(d);

  if(pre == 1) rn = up ? rn + rm : rn - rm;
  if(mode == 1) rd = load(Half | Nonsequential, rn);
  if(mode == 0) store(Half | Nonsequential, rn, rd);
  if(pre == 0) rn = up ? rn + rm : rn - rm;

  if(pre == 0 || writeback) r(n) = rn;
  if(mode == 1) r(d) = rd;
}

auto ARM7TDMI::armInstructionMoveImmediateOffset
(nall::Natural<12> immediate, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> mode, nall::Natural< 1> writeback, nall::Natural< 1> byte, nall::Natural< 1> up, nall::Natural< 1> pre) -> void {
  uint32_t rn = r(n);
  uint32_t rd = r(d);

  if(pre == 1) rn = up ? rn + immediate : rn - immediate;
  if(mode == 1) rd = load((byte ? Byte : Word) | Nonsequential, rn);
  if(mode == 0) store((byte ? Byte : Word) | Nonsequential, rn, rd);
  if(pre == 0) rn = up ? rn + immediate : rn - immediate;

  if(pre == 0 || writeback) r(n) = rn;
  if(mode == 1) r(d) = rd;
}

auto ARM7TDMI::armInstructionMoveMultiple
(uint16_t list, nall::Natural< 4> n, nall::Natural< 1> mode, nall::Natural< 1> writeback, nall::Natural< 1> type, nall::Natural< 1> up, nall::Natural< 1> pre) -> void {
  uint32_t rn = r(n);
  if(pre == 0 && up == 1) rn = rn + 0;  //IA
  if(pre == 1 && up == 1) rn = rn + 4;  //IB
  if(pre == 1 && up == 0) rn = rn - nall::count(list) * 4 + 0;  //DB
  if(pre == 0 && up == 0) rn = rn - nall::count(list) * 4 + 4;  //DA

  if(writeback && mode == 1) {
    if(up == 1) r(n) = r(n) + nall::count(list) * 4;  //IA,IB
    if(up == 0) r(n) = r(n) - nall::count(list) * 4;  //DA,DB
  }

  auto cpsrMode = cpsr().m;
  bool usr = false;
  if(type && mode == 1 && !(list & 0x8000)) usr = true;
  if(type && mode == 0) usr = true;
  if(usr) cpsr().m = PSR::USR;

  unsigned sequential = Nonsequential;
  for(unsigned m : range(16)) {
    if(!(list & 1 << m)) continue;
    if(mode == 1) r(m) = read(Word | sequential, rn);
    if(mode == 0) write(Word | sequential, rn, r(m));
    rn += 4;
    sequential = Sequential;
  }

  if(usr) cpsr().m = cpsrMode;

  if(mode) {
    idle();
    if(type && (list & 0x8000) && cpsr().m != PSR::USR && cpsr().m != PSR::SYS) {
      cpsr() = spsr();
    }
  } else {
    pipeline.nonsequential = true;
  }

  if(writeback && mode == 0) {
    if(up == 1) r(n) = r(n) + nall::count(list) * 4;  //IA,IB
    if(up == 0) r(n) = r(n) - nall::count(list) * 4;  //DA,DB
  }
}

auto ARM7TDMI::armInstructionMoveRegisterOffset
(nall::Natural< 4> m, nall::Natural< 2> type, nall::Natural< 5> shift, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> mode, nall::Natural< 1> writeback, nall::Natural< 1> byte, nall::Natural< 1> up, nall::Natural< 1> pre) -> void {
  uint32_t rm = r(m);
  uint32_t rd = r(d);
  uint32_t rn = r(n);
  carry = cpsr().c;

  switch(type) {
  case 0: rm = LSL(rm, shift); break;
  case 1: rm = LSR(rm, shift ? (unsigned)shift : 32); break;
  case 2: rm = ASR(rm, shift ? (unsigned)shift : 32); break;
  case 3: rm = shift ? ROR(rm, shift) : RRX(rm); break;
  }

  if(pre == 1) rn = up ? rn + rm : rn - rm;
  if(mode == 1) rd = load((byte ? Byte : Word) | Nonsequential, rn);
  if(mode == 0) store((byte ? Byte : Word) | Nonsequential, rn, rd);
  if(pre == 0) rn = up ? rn + rm : rn - rm;

  if(pre == 0 || writeback) r(n) = rn;
  if(mode == 1) r(d) = rd;
}

auto ARM7TDMI::armInstructionMoveToRegisterFromStatus
(nall::Natural< 4> d, nall::Natural< 1> mode) -> void {
  if(mode && (cpsr().m == PSR::USR || cpsr().m == PSR::SYS)) return;
  r(d) = mode ? spsr() : cpsr();
}

auto ARM7TDMI::armInstructionMoveToStatusFromImmediate
(uint8_t immediate, nall::Natural< 4> rotate, nall::Natural< 4> field, nall::Natural< 1> mode) -> void {
  uint32_t data = immediate;
  if(rotate) data = ROR(data, rotate << 1);
  armMoveToStatus(field, mode, data);
}

auto ARM7TDMI::armInstructionMoveToStatusFromRegister
(nall::Natural< 4> m, nall::Natural< 4> field, nall::Natural< 1> mode) -> void {
  armMoveToStatus(field, mode, r(m));
}

auto ARM7TDMI::armInstructionMultiply
(nall::Natural< 4> m, nall::Natural< 4> s, nall::Natural< 4> n, nall::Natural< 4> d, nall::Natural< 1> save, nall::Natural< 1> accumulate) -> void {
  if(accumulate) idle();
  r(d) = MUL(accumulate ? r(n) : 0, r(m), r(s));
}

auto ARM7TDMI::armInstructionMultiplyLong
(nall::Natural< 4> m, nall::Natural< 4> s, nall::Natural< 4> l, nall::Natural< 4> h, nall::Natural< 1> save, nall::Natural< 1> accumulate, nall::Natural< 1> sign) -> void {
  uint64_t rm = r(m);
  uint64_t rs = r(s);

  idle();
  idle();
  if(accumulate) idle();

  if(sign) {
    if(rs >>  8 && rs >>  8 != 0xffffff) idle();
    if(rs >> 16 && rs >> 16 !=   0xffff) idle();
    if(rs >> 24 && rs >> 24 !=     0xff) idle();
    rm = (int32_t)rm;
    rs = (int32_t)rs;
  } else {
    if(rs >>  8) idle();
    if(rs >> 16) idle();
    if(rs >> 24) idle();
  }

  uint64_t rd = rm * rs;
  if(accumulate) rd += (uint64_t)r(h) << 32 | (uint64_t)r(l) << 0;

  r(h) = rd >> 32;
  r(l) = rd >>  0;

  if(save) {
    cpsr().z = rd == 0;
    cpsr().n = rd >> 63 & 1;
  }
}

auto ARM7TDMI::armInstructionSoftwareInterrupt
(nall::Natural<24> immediate) -> void {
  exception(PSR::SVC, 0x08);
}

auto ARM7TDMI::armInstructionUndefined
() -> void {
  exception(PSR::UND, 0x04);
}

auto ARM7TDMI::thumbInstructionALU
(nall::Natural< 3> d, nall::Natural< 3> m, nall::Natural< 4> mode) -> void {
  switch(mode) {
  case  0: r(d) = BIT(r(d) & r(m)); break;  //AND
  case  1: r(d) = BIT(r(d) ^ r(m)); break;  //EOR
  case  2: r(d) = BIT(LSL(r(d), r(m))); break;  //LSL
  case  3: r(d) = BIT(LSR(r(d), r(m))); break;  //LSR
  case  4: r(d) = BIT(ASR(r(d), r(m))); break;  //ASR
  case  5: r(d) = ADD(r(d), r(m), cpsr().c); break;  //ADC
  case  6: r(d) = SUB(r(d), r(m), cpsr().c); break;  //SBC
  case  7: r(d) = BIT(ROR(r(d), r(m))); break;  //ROR
  case  8:        BIT(r(d) & r(m)); break;  //TST
  case  9: r(d) = SUB(0, r(m), 1); break;  //NEG
  case 10:        SUB(r(d), r(m), 1); break;  //CMP
  case 11:        ADD(r(d), r(m), 0); break;  //CMN
  case 12: r(d) = BIT(r(d) | r(m)); break;  //ORR
  case 13: r(d) = MUL(0, r(m), r(d)); break;  //MUL
  case 14: r(d) = BIT(r(d) & ~r(m)); break;  //BIC
  case 15: r(d) = BIT(~r(m)); break;  //MVN
  }
}

auto ARM7TDMI::thumbInstructionALUExtended
(nall::Natural< 4> d, nall::Natural< 4> m, nall::Natural< 2> mode) -> void {
  switch(mode) {
  case 0: r(d) = r(d) + r(m); break;  //ADD
  case 1: SUB(r(d), r(m), 1); break;  //SUBS
  case 2: r(d) = r(m); break;  //MOV
  }
}

auto ARM7TDMI::thumbInstructionAddRegister
(uint8_t immediate, nall::Natural< 3> d, nall::Natural< 1> mode) -> void {
  switch(mode) {
  case 0: r(d) = (r(15) & ~3) + immediate * 4; break;  //ADD pc
  case 1: r(d) = r(13) + immediate * 4; break;  //ADD sp
  }
}

auto ARM7TDMI::thumbInstructionAdjustImmediate
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 3> immediate, nall::Natural< 1> mode) -> void {
  switch(mode) {
  case 0: r(d) = ADD(r(n), immediate, 0); break;  //ADD
  case 1: r(d) = SUB(r(n), immediate, 1); break;  //SUB
  }
}

auto ARM7TDMI::thumbInstructionAdjustRegister
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 3> m, nall::Natural< 1> mode) -> void {
  switch(mode) {
  case 0: r(d) = ADD(r(n), r(m), 0); break;  //ADD
  case 1: r(d) = SUB(r(n), r(m), 1); break;  //SUB
  }
}

auto ARM7TDMI::thumbInstructionAdjustStack
(nall::Natural< 7> immediate, nall::Natural< 1> mode) -> void {
  switch(mode) {
  case 0: r(13) = r(13) + immediate * 4; break;  //ADD
  case 1: r(13) = r(13) - immediate * 4; break;  //SUB
  }
}

auto ARM7TDMI::thumbInstructionBranchExchange
(nall::Natural< 4> m) -> void {
  uint32_t address = r(m);
  cpsr().t = address & 1;
  r(15) = address;
}

auto ARM7TDMI::thumbInstructionBranchFarPrefix
(nall::Integer<11> displacement) -> void {
  r(14) = r(15) + (displacement * 2 << 11);
}

auto ARM7TDMI::thumbInstructionBranchFarSuffix
(nall::Natural<11> displacement) -> void {
  r(15) = r(14) + (displacement * 2);
  r(14) = pipeline.decode.address | 1;
}

auto ARM7TDMI::thumbInstructionBranchNear
(nall::Integer<11> displacement) -> void {
  r(15) = r(15) + displacement * 2;
}

auto ARM7TDMI::thumbInstructionBranchTest
(int8_t displacement, nall::Natural< 4> condition) -> void {
  if(!TST(condition)) return;
  r(15) = r(15) + displacement * 2;
}

auto ARM7TDMI::thumbInstructionImmediate
(uint8_t immediate, nall::Natural< 3> d, nall::Natural< 2> mode) -> void {
  switch(mode) {
  case 0: r(d) = BIT(immediate); break;  //MOV
  case 1:        SUB(r(d), immediate, 1); break;  //CMP
  case 2: r(d) = ADD(r(d), immediate, 0); break;  //ADD
  case 3: r(d) = SUB(r(d), immediate, 1); break;  //SUB
  }
}

auto ARM7TDMI::thumbInstructionLoadLiteral
(uint8_t displacement, nall::Natural< 3> d) -> void {
  uint32_t address = (r(15) & ~3) + (displacement << 2);
  r(d) = load(Word | Nonsequential, address);
}

auto ARM7TDMI::thumbInstructionMoveByteImmediate
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 5> offset, nall::Natural< 1> mode) -> void {
  switch(mode) {
  case 0: store(Byte | Nonsequential, r(n) + offset, r(d)); break;  //STRB
  case 1: r(d) = load(Byte | Nonsequential, r(n) + offset); break;  //LDRB
  }
}

auto ARM7TDMI::thumbInstructionMoveHalfImmediate
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 5> offset, nall::Natural< 1> mode) -> void {
  switch(mode) {
  case 0: store(Half | Nonsequential, r(n) + offset * 2, r(d)); break;  //STRH
  case 1: r(d) = load(Half | Nonsequential, r(n) + offset * 2); break;  //LDRH
  }
}

auto ARM7TDMI::thumbInstructionMoveMultiple
(uint8_t list, nall::Natural< 3> n, nall::Natural< 1> mode) -> void {
  uint32_t rn = r(n);

  for(unsigned m : range(8)) {
    if(!(list & 1 << m)) continue;
    switch(mode) {
    case 0: write(Word | Nonsequential, rn, r(m)); break;  //STMIA
    case 1: r(m) = read(Word | Nonsequential, rn); break;  //LDMIA
    }
    rn += 4;
  }

  if(mode == 0 || !(list & 1 << n)) r(n) = rn;
  if(mode == 1) idle();
}

auto ARM7TDMI::thumbInstructionMoveRegisterOffset
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 3> m, nall::Natural< 3> mode) -> void {
  switch(mode) {
  case 0: store(Word | Nonsequential, r(n) + r(m), r(d)); break;  //STR
  case 1: store(Half | Nonsequential, r(n) + r(m), r(d)); break;  //STRH
  case 2: store(Byte | Nonsequential, r(n) + r(m), r(d)); break;  //STRB
  case 3: r(d) = load(Byte | Nonsequential | Signed, r(n) + r(m)); break;  //LDSB
  case 4: r(d) = load(Word | Nonsequential, r(n) + r(m)); break;  //LDR
  case 5: r(d) = load(Half | Nonsequential, r(n) + r(m)); break;  //LDRH
  case 6: r(d) = load(Byte | Nonsequential, r(n) + r(m)); break;  //LDRB
  case 7: r(d) = load(Half | Nonsequential | Signed, r(n) + r(m)); break;  //LDSH
  }
}

auto ARM7TDMI::thumbInstructionMoveStack
(uint8_t immediate, nall::Natural< 3> d, nall::Natural< 1> mode) -> void {
  switch(mode) {
  case 0: store(Word | Nonsequential, r(13) + immediate * 4, r(d)); break;  //STR
  case 1: r(d) = load(Word | Nonsequential, r(13) + immediate * 4); break;  //LDR
  }
}

auto ARM7TDMI::thumbInstructionMoveWordImmediate
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 5> offset, nall::Natural< 1> mode) -> void {
  switch(mode) {
  case 0: store(Word | Nonsequential, r(n) + offset * 4, r(d)); break;  //STR
  case 1: r(d) = load(Word | Nonsequential, r(n) + offset * 4); break;  //LDR
  }
}

auto ARM7TDMI::thumbInstructionShiftImmediate
(nall::Natural< 3> d, nall::Natural< 3> m, nall::Natural< 5> immediate, nall::Natural< 2> mode) -> void {
  switch(mode) {
  case 0: r(d) = BIT(LSL(r(m), immediate)); break;  //LSL
  case 1: r(d) = BIT(LSR(r(m), immediate ? (unsigned)immediate : 32)); break;  //LSR
  case 2: r(d) = BIT(ASR(r(m), immediate ? (unsigned)immediate : 32)); break;  //ASR
  }
}

auto ARM7TDMI::thumbInstructionSoftwareInterrupt
(uint8_t immediate) -> void {
  exception(PSR::SVC, 0x08);
}

auto ARM7TDMI::thumbInstructionStackMultiple
(uint8_t list, nall::Natural< 1> lrpc, nall::Natural< 1> mode) -> void {
  uint32_t sp;
  switch(mode) {
  case 0: sp = r(13) - (nall::count(list) + lrpc) * 4; break;  //PUSH
  case 1: sp = r(13);  //POP
  }

  unsigned sequential = Nonsequential;
  for(unsigned m : range(8)) {
    if(!(list & 1 << m)) continue;
    switch(mode) {
    case 0: write(Word | sequential, sp, r(m)); break;  //PUSH
    case 1: r(m) = read(Word | sequential, sp); break;  //POP
    }
    sp += 4;
    sequential = Sequential;
  }

  if(lrpc) {
    switch(mode) {
    case 0: write(Word | sequential, sp, r(14)); break;  //PUSH
    case 1: r(15) = read(Word | sequential, sp); break;  //POP
    }
    sp += 4;
  }

  if(mode == 1) {
    idle();
    r(13) = r(13) + (nall::count(list) + lrpc) * 4;  //POP
  } else {
    pipeline.nonsequential = true;
    r(13) = r(13) - (nall::count(list) + lrpc) * 4;  //PUSH
  }
}

auto ARM7TDMI::thumbInstructionUndefined
() -> void {
  exception(PSR::UND, 0x04);
}

auto ARM7TDMI::serialize(serializer& s) -> void {
  processor.serialize(s);
  pipeline.serialize(s);
  s.boolean(carry);
  s.boolean(irq);
}

auto ARM7TDMI::Processor::serialize(serializer& s) -> void {
  s.integer(r0.data);
  s.integer(r1.data);
  s.integer(r2.data);
  s.integer(r3.data);
  s.integer(r4.data);
  s.integer(r5.data);
  s.integer(r6.data);
  s.integer(r7.data);
  s.integer(r8.data);
  s.integer(r9.data);
  s.integer(r10.data);
  s.integer(r11.data);
  s.integer(r12.data);
  s.integer(r13.data);
  s.integer(r14.data);
  s.integer(r15.data);
  cpsr.serialize(s);
  s.integer(fiq.r8.data);
  s.integer(fiq.r9.data);
  s.integer(fiq.r10.data);
  s.integer(fiq.r11.data);
  s.integer(fiq.r12.data);
  s.integer(fiq.r13.data);
  s.integer(fiq.r14.data);
  fiq.spsr.serialize(s);
  s.integer(irq.r13.data);
  s.integer(irq.r14.data);
  irq.spsr.serialize(s);
  s.integer(svc.r13.data);
  s.integer(svc.r14.data);
  svc.spsr.serialize(s);
  s.integer(abt.r13.data);
  s.integer(abt.r14.data);
  abt.spsr.serialize(s);
  s.integer(und.r13.data);
  s.integer(und.r14.data);
  und.spsr.serialize(s);
}

auto ARM7TDMI::PSR::serialize(serializer& s) -> void {
  s.integer(m);
  s.boolean(t);
  s.boolean(f);
  s.boolean(i);
  s.boolean(v);
  s.boolean(c);
  s.boolean(z);
  s.boolean(n);
}

auto ARM7TDMI::Pipeline::serialize(serializer& s) -> void {
  s.integer(reload);
  s.integer(nonsequential);
  s.integer(fetch.address);
  s.integer(fetch.instruction);
  s.boolean(fetch.thumb);
  s.integer(decode.address);
  s.integer(decode.instruction);
  s.boolean(decode.thumb);
  s.integer(execute.address);
  s.integer(execute.instruction);
  s.boolean(execute.thumb);
}

static const string _r[] = {
  "r0", "r1",  "r2",  "r3",  "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc"
};
static const string _conditions[] = {
  "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
  "hi", "ls", "ge", "lt", "gt", "le", "",   "nv",
};
#define _s save ? "s" : ""
#define _move(mode) (mode == 13 || mode == 15)
#define _comp(mode) (mode >=  8 && mode <= 11)
#define _math(mode) (mode <=  7 || mode == 12 || mode == 14)

auto ARM7TDMI::disassemble(maybe<uint32_t> pc, maybe<Boolean> thumb) -> string {
  if(!pc) pc = pipeline.execute.address;
  if(!thumb) thumb = cpsr().t;

  _pc = pc();
  if(!thumb()) {
    uint32_t opcode = read(Word | Nonsequential, _pc & ~3);
    nall::Natural<12> index = (opcode & 0x0ff00000) >> 16 | (opcode & 0x000000f0) >> 4;
    _c = _conditions[opcode >> 28];
    return {hex(_pc, 8L), "  ", armDisassemble[index](opcode)};
  } else {
    uint16_t opcode = read(Half | Nonsequential, _pc & ~1);
    return {hex(_pc, 8L), "  ", thumbDisassemble[opcode]()};
  }
}

auto ARM7TDMI::disassembleRegisters() -> string {
  string output;
  for(unsigned n : range(16)) {
    output.append(_r[n], ":", hex(r(n), 8L), " ");
  }

  output.append("cpsr:");
  output.append(cpsr().n ? "N" : "n");
  output.append(cpsr().z ? "Z" : "z");
  output.append(cpsr().c ? "C" : "c");
  output.append(cpsr().v ? "V" : "v", "/");
  output.append(cpsr().i ? "I" : "i");
  output.append(cpsr().f ? "F" : "f");
  output.append(cpsr().t ? "T" : "t", "/");
  output.append(hex(cpsr().m, 2L));
  if(cpsr().m == PSR::USR || cpsr().m == PSR::SYS) return output;

  output.append(" spsr:");
  output.append(spsr().n ? "N" : "n");
  output.append(spsr().z ? "Z" : "z");
  output.append(spsr().c ? "C" : "c");
  output.append(spsr().v ? "V" : "v", "/");
  output.append(spsr().i ? "I" : "i");
  output.append(spsr().f ? "F" : "f");
  output.append(spsr().t ? "T" : "t", "/");
  output.append(hex(spsr().m, 2L));
  return output;
}

//

auto ARM7TDMI::armDisassembleBranch
(nall::Integer<24> displacement, nall::Natural< 1> link) -> string {
  return {"b", link ? "l" : "", _c, " 0x", hex(_pc + 8 + displacement * 4, 8L)};
}

auto ARM7TDMI::armDisassembleBranchExchangeRegister
(nall::Natural< 4> m) -> string {
  return {"bx", _c, " ", _r[m]};
}

auto ARM7TDMI::armDisassembleDataImmediate
(uint8_t immediate, nall::Natural< 4> shift, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> save, nall::Natural< 4> mode) -> string {
  static const string opcode[] = {
    "and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
    "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn",
  };
  uint32_t data = immediate >> (shift << 1) | immediate << 32 - (shift << 1);
  return {opcode[mode], _c,
    _move(mode) ? string{_s, " ", _r[d]} : string{},
    _comp(mode) ? string{" ", _r[n]} : string{},
    _math(mode) ? string{_s, " ", _r[d], ",", _r[n]} : string{},
    ",#0x", hex(data, 8L)};
}

auto ARM7TDMI::armDisassembleDataImmediateShift
(nall::Natural< 4> m, nall::Natural< 2> type, nall::Natural< 5> shift, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> save, nall::Natural< 4> mode) -> string {
  static const string opcode[] = {
    "and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
    "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn",
  };
  return {opcode[mode], _c,
    _move(mode) ? string{_s, " ", _r[d]} : string{},
    _comp(mode) ? string{" ", _r[n]} : string{},
    _math(mode) ? string{_s, " ", _r[d], ",", _r[n]} : string{},
    ",", _r[m],
    type == 0 && shift ? string{" lsl #", shift} : string{},
    type == 1 ? string{" lsr #", shift ? (unsigned)shift : 32} : string{},
    type == 2 ? string{" asr #", shift ? (unsigned)shift : 32} : string{},
    type == 3 && shift ? string{" ror #", shift} : string{},
    type == 3 && !shift ? " rrx" : ""};
}

auto ARM7TDMI::armDisassembleDataRegisterShift
(nall::Natural< 4> m, nall::Natural< 2> type, nall::Natural< 4> s, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> save, nall::Natural< 4> mode) -> string {
  static const string opcode[] = {
    "and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
    "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn",
  };
  return {opcode[mode], _c,
    _move(mode) ? string{_s, " ", _r[d]} : string{},
    _comp(mode) ? string{" ", _r[n]} : string{},
    _math(mode) ? string{_s, " ", _r[d], ",", _r[n]} : string{},
    ",", _r[m], " ",
    type == 0 ? "lsl" : "",
    type == 1 ? "lsr" : "",
    type == 2 ? "asr" : "",
    type == 3 ? "ror" : "",
    " ", _r[s]};
}

auto ARM7TDMI::armDisassembleLoadImmediate
(uint8_t immediate, nall::Natural< 1> half, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> writeback, nall::Natural< 1> up, nall::Natural< 1> pre) -> string {
  string data;
  if(n == 15) data = {" =0x", hex(read((half ? Half : Byte) | Nonsequential,
    _pc + 8 + (up ? +immediate : -immediate)), half ? 4L : 2L)};

  return {"ldr", _c, half ? "sh" : "sb", " ",
    _r[d], ",[", _r[n],
    pre == 0 ? "]" : "",
    immediate ? string{",", up ? "+" : "-", "0x", hex(immediate, 2L)} : string{},
    pre == 1 ? "]" : "",
    pre == 0 || writeback ? "!" : "", data};
}

auto ARM7TDMI::armDisassembleLoadRegister
(nall::Natural< 4> m, nall::Natural< 1> half, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> writeback, nall::Natural< 1> up, nall::Natural< 1> pre) -> string {
  return {"ldr", _c, half ? "sh" : "sb", " ",
    _r[d], ",[", _r[n],
    pre == 0 ? "]" : "",
    ",", up ? "+" : "-", _r[m],
    pre == 1 ? "]" : "",
    pre == 0 || writeback ? "!" : ""};
}

auto ARM7TDMI::armDisassembleMemorySwap
(nall::Natural< 4> m, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> byte) -> string {
  return {"swp", _c, byte ? "b" : "", " ", _r[d], ",", _r[m], ",[", _r[n], "]"};
}

auto ARM7TDMI::armDisassembleMoveHalfImmediate
(uint8_t immediate, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> mode, nall::Natural< 1> writeback, nall::Natural< 1> up, nall::Natural< 1> pre) -> string {
  string data;
  if(n == 15) data = {" =0x", hex(read(Half | Nonsequential, _pc + (up ? +immediate : -immediate)), 4L)};

  return {mode ? "ldr" : "str", _c, "h ",
    _r[d], ",[", _r[n],
    pre == 0 ? "]" : "",
    immediate ? string{",", up ? "+" : "-", "0x", hex(immediate, 2L)} : string{},
    pre == 1 ? "]" : "",
    pre == 0 || writeback ? "!" : "", data};
}

auto ARM7TDMI::armDisassembleMoveHalfRegister
(nall::Natural< 4> m, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> mode, nall::Natural< 1> writeback, nall::Natural< 1> up, nall::Natural< 1> pre) -> string {
  return {mode ? "ldr" : "str", _c, "h ",
    _r[d], ",[", _r[n],
    pre == 0 ? "]" : "",
    ",", up ? "+" : "-", _r[m],
    pre == 1 ? "]" : "",
    pre == 0 || writeback ? "!" : ""};
}

auto ARM7TDMI::armDisassembleMoveImmediateOffset
(nall::Natural<12> immediate, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> mode, nall::Natural< 1> writeback, nall::Natural< 1> byte, nall::Natural< 1> up, nall::Natural< 1> pre) -> string {
  string data;
  if(n == 15) data = {" =0x", hex(read((byte ? Byte : Word) | Nonsequential,
    _pc + 8 + (up ? +immediate : -immediate)), byte ? 2L : 4L)};
  return {mode ? "ldr" : "str", _c, byte ? "b" : "", " ", _r[d], ",[", _r[n],
    pre == 0 ? "]" : "",
    immediate ? string{",", up ? "+" : "-", "0x", hex(immediate, 3L)} : string{},
    pre == 1 ? "]" : "",
    pre == 0 || writeback ? "!" : "", data};
}

auto ARM7TDMI::armDisassembleMoveMultiple
(uint16_t list, nall::Natural< 4> n, nall::Natural< 1> mode, nall::Natural< 1> writeback, nall::Natural< 1> type, nall::Natural< 1> up, nall::Natural< 1> pre) -> string {
  string registers;
  for(auto index : range(16)) {
    if((list & 1 << index)) registers.append(_r[index], ",");
  }
  registers.trimRight(",", 1L);
  return {mode ? "ldm" : "stm", _c,
    up == 0 && pre == 0 ? "da" : "",
    up == 0 && pre == 1 ? "db" : "",
    up == 1 && pre == 0 ? "ia" : "",
    up == 1 && pre == 1 ? "ib" : "",
    " ", _r[n], writeback ? "!" : "",
    ",{", registers, "}", type ? "^" : ""};
}

auto ARM7TDMI::armDisassembleMoveRegisterOffset
(nall::Natural< 4> m, nall::Natural< 2> type, nall::Natural< 5> shift, nall::Natural< 4> d, nall::Natural< 4> n, nall::Natural< 1> mode, nall::Natural< 1> writeback, nall::Natural< 1> byte, nall::Natural< 1> up, nall::Natural< 1> pre) -> string {
  return {mode ? "ldr" : "str", _c, byte ? "b" : "", " ", _r[d], ",[", _r[n],
    pre == 0 ? "]" : "",
    ",", up ? "+" : "-", _r[m],
    type == 0 && shift ? string{" lsl #", shift} : string{},
    type == 1 ? string{" lsr #", shift ? (unsigned)shift : 32} : string{},
    type == 2 ? string{" asr #", shift ? (unsigned)shift : 32} : string{},
    type == 3 && shift ? string{" ror #", shift} : string{},
    type == 3 && !shift ? " rrx" : "",
    pre == 1 ? "]" : "",
    pre == 0 || writeback ? "!" : ""};
}

auto ARM7TDMI::armDisassembleMoveToRegisterFromStatus
(nall::Natural< 4> d, nall::Natural< 1> mode) -> string {
  return {"mrs", _c, " ", _r[d], ",", mode ? "spsr" : "cpsr"};
}

auto ARM7TDMI::armDisassembleMoveToStatusFromImmediate
(uint8_t immediate, nall::Natural< 4> rotate, nall::Natural< 4> field, nall::Natural< 1> mode) -> string {
  uint32_t data = immediate >> (rotate << 1) | immediate << 32 - (rotate << 1);
  return {"msr", _c, " ", mode ? "spsr:" : "cpsr:",
    field.bit(0) ? "c" : "",
    field.bit(1) ? "x" : "",
    field.bit(2) ? "s" : "",
    field.bit(3) ? "f" : "",
    ",#0x", hex(data, 8L)};
}

auto ARM7TDMI::armDisassembleMoveToStatusFromRegister
(nall::Natural< 4> m, nall::Natural< 4> field, nall::Natural< 1> mode) -> string {
  return {"msr", _c, " ", mode ? "spsr:" : "cpsr:",
    field.bit(0) ? "c" : "",
    field.bit(1) ? "x" : "",
    field.bit(2) ? "s" : "",
    field.bit(3) ? "f" : "",
    ",", _r[m]};
}

auto ARM7TDMI::armDisassembleMultiply
(nall::Natural< 4> m, nall::Natural< 4> s, nall::Natural< 4> n, nall::Natural< 4> d, nall::Natural< 1> save, nall::Natural< 1> accumulate) -> string {
  if(accumulate) {
    return {"mla", _c, _s, " ", _r[d], ",", _r[m], ",", _r[s], ",", _r[n]};
  } else {
    return {"mul", _c, _s, " ", _r[d], ",", _r[m], ",", _r[s]};
  }
}

auto ARM7TDMI::armDisassembleMultiplyLong
(nall::Natural< 4> m, nall::Natural< 4> s, nall::Natural< 4> l, nall::Natural< 4> h, nall::Natural< 1> save, nall::Natural< 1> accumulate, nall::Natural< 1> sign) -> string {
  return {sign ? "s" : "u", accumulate ? "mlal" : "mull", _c, _s, " ",
    _r[l], ",", _r[h], ",", _r[m], ",", _r[s]};
}

auto ARM7TDMI::armDisassembleSoftwareInterrupt
(nall::Natural<24> immediate) -> string {
  return {"swi #0x", hex(immediate, 6L)};
}

auto ARM7TDMI::armDisassembleUndefined
() -> string {
  return {"undefined"};
}

//

auto ARM7TDMI::thumbDisassembleALU
(nall::Natural< 3> d, nall::Natural< 3> m, nall::Natural< 4> mode) -> string {
  static const string opcode[] = {
    "and", "eor", "lsl", "lsr", "asr", "adc", "sbc", "ror",
    "tst", "neg", "cmp", "cmn", "orr", "mul", "bic", "mvn",
  };
  return {opcode[mode], " ", _r[d], ",", _r[m]};
}

auto ARM7TDMI::thumbDisassembleALUExtended
(nall::Natural< 4> d, nall::Natural< 4> m, nall::Natural< 2> mode) -> string {
  static const string opcode[] = {"add", "sub", "mov"};
  if(d == 8 && m == 8 && mode == 2) return {"nop"};
  return {opcode[mode], " ", _r[d], ",", _r[m]};
}

auto ARM7TDMI::thumbDisassembleAddRegister
(uint8_t immediate, nall::Natural< 3> d, nall::Natural< 1> mode) -> string {
  return {"add ", _r[d], ",", mode ? "sp" : "pc", ",#0x", hex(immediate, 2L)};
}

auto ARM7TDMI::thumbDisassembleAdjustImmediate
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 3> immediate, nall::Natural< 1> mode) -> string {
  return {!mode ? "add" : "sub", " ", _r[d], ",", _r[n], ",#", immediate};
}

auto ARM7TDMI::thumbDisassembleAdjustRegister
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 3> m, nall::Natural< 1> mode) -> string {
  return {!mode ? "add" : "sub", " ", _r[d], ",", _r[n], ",", _r[m]};
}

auto ARM7TDMI::thumbDisassembleAdjustStack
(nall::Natural< 7> immediate, nall::Natural< 1> mode) -> string {
  return {!mode ? "add" : "sub", " sp,#0x", hex(immediate * 4, 3L)};
}

auto ARM7TDMI::thumbDisassembleBranchExchange
(nall::Natural< 4> m) -> string {
  return {"bx ", _r[m]};
}

auto ARM7TDMI::thumbDisassembleBranchFarPrefix
(nall::Integer<11> displacementHi) -> string {
  nall::Natural<11> displacementLo = read(Half | Nonsequential, (_pc & ~1) + 2);
  nall::Integer<22> displacement = displacementHi << 11 | displacementLo << 0;
  uint32_t address = _pc + 4 + displacement * 2;
  return {"bl 0x", hex(address, 8L)};
}

auto ARM7TDMI::thumbDisassembleBranchFarSuffix
(nall::Natural<11> displacement) -> string {
  return {"bl (suffix)"};
}

auto ARM7TDMI::thumbDisassembleBranchNear
(nall::Integer<11> displacement) -> string {
  uint32_t address = _pc + 4 + displacement * 2;
  return {"b 0x", hex(address, 8L)};
}

auto ARM7TDMI::thumbDisassembleBranchTest
(int8_t displacement, nall::Natural< 4> condition) -> string {
  uint32_t address = _pc + 4 + displacement * 2;
  return {"b", _conditions[condition], " 0x", hex(address, 8L)};
}

auto ARM7TDMI::thumbDisassembleImmediate
(uint8_t immediate, nall::Natural< 3> d, nall::Natural< 2> mode) -> string {
  static const string opcode[] = {"mov", "cmp", "add", "sub"};
  return {opcode[mode], " ", _r[d], ",#0x", hex(immediate, 2L)};
}

auto ARM7TDMI::thumbDisassembleLoadLiteral
(uint8_t displacement, nall::Natural< 3> d) -> string {
  uint32_t address = ((_pc + 4) & ~3) + (displacement << 2);
  uint32_t data = read(Word | Nonsequential, address);
  return {"ldr ", _r[d], ",[pc,#0x", hex(address, 8L), "] =0x", hex(data, 8L)};
}

auto ARM7TDMI::thumbDisassembleMoveByteImmediate
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 5> offset, nall::Natural< 1> mode) -> string {
  return {mode ? "ldrb" : "strb", " ", _r[d], ",[", _r[n], ",#0x", hex(offset, 2L), "]"};
}

auto ARM7TDMI::thumbDisassembleMoveHalfImmediate
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 5> offset, nall::Natural< 1> mode) -> string {
  return {mode ? "ldrh" : "strh", " ", _r[d], ",[", _r[n], ",#0x", hex(offset * 2, 2L), "]"};
}

auto ARM7TDMI::thumbDisassembleMoveMultiple
(uint8_t list, nall::Natural< 3> n, nall::Natural< 1> mode) -> string {
  string registers;
  for(unsigned m : range(8)) {
    if((list & 1 << m)) registers.append(_r[m], ",");
  }
  registers.trimRight(",", 1L);
  return {mode ? "ldmia" : "stmia", " ", _r[n], "!,{", registers, "}"};
}

auto ARM7TDMI::thumbDisassembleMoveRegisterOffset
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 3> m, nall::Natural< 3> mode) -> string {
  static const string opcode[] = {"str", "strh", "strb", "ldsb", "ldr", "ldrh", "ldrb", "ldsh"};
  return {opcode[mode], " ", _r[d], ",[", _r[n], ",", _r[m], "]"};
}

auto ARM7TDMI::thumbDisassembleMoveStack
(uint8_t immediate, nall::Natural< 3> d, nall::Natural< 1> mode) -> string {
  return {mode ? "ldr" : "str", " ", _r[d], ",[sp,#0x", hex(immediate * 4, 3L), "]"};
}

auto ARM7TDMI::thumbDisassembleMoveWordImmediate
(nall::Natural< 3> d, nall::Natural< 3> n, nall::Natural< 5> offset, nall::Natural< 1> mode) -> string {
  return {mode ? "ldr" : "str", " ", _r[d], ",[", _r[n], ",#0x", hex(offset * 4, 2L), "]"};
}

auto ARM7TDMI::thumbDisassembleShiftImmediate
(nall::Natural< 3> d, nall::Natural< 3> m, nall::Natural< 5> immediate, nall::Natural< 2> mode) -> string {
  static const string opcode[] = {"lsl", "lsr", "asr"};
  return {opcode[mode], " ", _r[d], ",", _r[m], ",#", immediate};
}

auto ARM7TDMI::thumbDisassembleSoftwareInterrupt
(uint8_t immediate) -> string {
  return {"swi #0x", hex(immediate, 2L)};
}

auto ARM7TDMI::thumbDisassembleStackMultiple
(uint8_t list, nall::Natural< 1> lrpc, nall::Natural< 1> mode) -> string {
  string registers;
  for(unsigned m : range(8)) {
    if((list & 1 << m)) registers.append(_r[m], ",");
  }
  if(lrpc) registers.append(!mode ? "lr," : "pc,");
  registers.trimRight(",", 1L);
  return {!mode ? "push" : "pop", " {", registers, "}"};
}

auto ARM7TDMI::thumbDisassembleUndefined
() -> string {
  return {"undefined"};
}

#undef _s
#undef _move
#undef _comp
#undef _save

ARM7TDMI::ARM7TDMI() {
  armInitialize();
  thumbInitialize();
}

auto ARM7TDMI::power() -> void {
  processor = {};
  processor.r15.modify = [&] { pipeline.reload = true; };
  pipeline = {};
  carry = 0;
  irq = 0;
  cpsr().f = 1;
  exception(PSR::SVC, 0x00);
}

}
