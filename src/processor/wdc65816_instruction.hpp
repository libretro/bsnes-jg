switch(fetch()) {
  //emulation mode lacks BRK vector; uses IRQ vector instead
  case 0x00: return instructionInterrupt(EF ? (r16)0xfffe : (r16)0xffe6);
  case 0x01: return MF ? instructionIndexedIndirectRead8(&WDC65816::algorithmORA8) : instructionIndexedIndirectRead16(&WDC65816::algorithmORA16);
  case 0x02: return instructionInterrupt(EF ? (r16)0xfff4 : (r16)0xffe4);
  case 0x03: return MF ? instructionStackRead8(&WDC65816::algorithmORA8) : instructionStackRead16(&WDC65816::algorithmORA16);
  case 0x04: return MF ? instructionDirectModify8(&WDC65816::algorithmTSB8) : instructionDirectModify16(&WDC65816::algorithmTSB16);
  case 0x05: return MF ? instructionDirectRead8(&WDC65816::algorithmORA8) : instructionDirectRead16(&WDC65816::algorithmORA16);
  case 0x06: return MF ? instructionDirectModify8(&WDC65816::algorithmASL8) : instructionDirectModify16(&WDC65816::algorithmASL16);
  case 0x07: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmORA8) : instructionIndirectLongRead16(&WDC65816::algorithmORA16);
  case 0x08: return instructionPush8((r16)P);
  case 0x09: return MF ? instructionImmediateRead8(&WDC65816::algorithmORA8) : instructionImmediateRead16(&WDC65816::algorithmORA16);
  case 0x0a: return MF ? instructionImpliedModify8(&WDC65816::algorithmASL8, A) : instructionImpliedModify16(&WDC65816::algorithmASL16, A);
  case 0x0b: return instructionPushD();
  case 0x0c: return MF ? instructionBankModify8(&WDC65816::algorithmTSB8) : instructionBankModify16(&WDC65816::algorithmTSB16);
  case 0x0d: return MF ? instructionBankRead8(&WDC65816::algorithmORA8) : instructionBankRead16(&WDC65816::algorithmORA16);
  case 0x0e: return MF ? instructionBankModify8(&WDC65816::algorithmASL8) : instructionBankModify16(&WDC65816::algorithmASL16);
  case 0x0f: return MF ? instructionLongRead8(&WDC65816::algorithmORA8) : instructionLongRead16(&WDC65816::algorithmORA16);
  case 0x10: return instructionBranch(NF == 0);
  case 0x11: return MF ? instructionIndirectIndexedRead8(&WDC65816::algorithmORA8) : instructionIndirectIndexedRead16(&WDC65816::algorithmORA16);
  case 0x12: return MF ? instructionIndirectRead8(&WDC65816::algorithmORA8) : instructionIndirectRead16(&WDC65816::algorithmORA16);
  case 0x13: return MF ? instructionIndirectStackRead8(&WDC65816::algorithmORA8) : instructionIndirectStackRead16(&WDC65816::algorithmORA16);
  case 0x14: return MF ? instructionDirectModify8(&WDC65816::algorithmTRB8) : instructionDirectModify16(&WDC65816::algorithmTRB16);
  case 0x15: return MF ? instructionDirectRead8(&WDC65816::algorithmORA8, X) : instructionDirectRead16(&WDC65816::algorithmORA16, X);
  case 0x16: return MF ? instructionDirectIndexedModify8(&WDC65816::algorithmASL8) : instructionDirectIndexedModify16(&WDC65816::algorithmASL16);
  case 0x17: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmORA8, Y) : instructionIndirectLongRead16(&WDC65816::algorithmORA16, Y);
  case 0x18: return instructionClearFlag(CF);
  case 0x19: return MF ? instructionBankRead8(&WDC65816::algorithmORA8, Y) : instructionBankRead16(&WDC65816::algorithmORA16, Y);
  case 0x1a: return MF ? instructionImpliedModify8(&WDC65816::algorithmINC8, A) : instructionImpliedModify16(&WDC65816::algorithmINC16, A);
  case 0x1b: return instructionTransferCS();
  case 0x1c: return MF ? instructionBankModify8(&WDC65816::algorithmTRB8) : instructionBankModify16(&WDC65816::algorithmTRB16);
  case 0x1d: return MF ? instructionBankRead8(&WDC65816::algorithmORA8, X) : instructionBankRead16(&WDC65816::algorithmORA16, X);
  case 0x1e: return MF ? instructionBankIndexedModify8(&WDC65816::algorithmASL8) : instructionBankIndexedModify16(&WDC65816::algorithmASL16);
  case 0x1f: return MF ? instructionLongRead8(&WDC65816::algorithmORA8, X) : instructionLongRead16(&WDC65816::algorithmORA16, X);
  case 0x20: return instructionCallShort();
  case 0x21: return MF ? instructionIndexedIndirectRead8(&WDC65816::algorithmAND8) : instructionIndexedIndirectRead16(&WDC65816::algorithmAND16);
  case 0x22: return instructionCallLong();
  case 0x23: return MF ? instructionStackRead8(&WDC65816::algorithmAND8) : instructionStackRead16(&WDC65816::algorithmAND16);
  case 0x24: return MF ? instructionDirectRead8(&WDC65816::algorithmBIT8) : instructionDirectRead16(&WDC65816::algorithmBIT16);
  case 0x25: return MF ? instructionDirectRead8(&WDC65816::algorithmAND8) : instructionDirectRead16(&WDC65816::algorithmAND16);
  case 0x26: return MF ? instructionDirectModify8(&WDC65816::algorithmROL8) : instructionDirectModify16(&WDC65816::algorithmROL16);
  case 0x27: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmAND8) : instructionIndirectLongRead16(&WDC65816::algorithmAND16);
  case 0x28: return instructionPullP();
  case 0x29: return MF ? instructionImmediateRead8(&WDC65816::algorithmAND8) : instructionImmediateRead16(&WDC65816::algorithmAND16);
  case 0x2a: return MF ? instructionImpliedModify8(&WDC65816::algorithmROL8, A) : instructionImpliedModify16(&WDC65816::algorithmROL16, A);
  case 0x2b: return instructionPullD();
  case 0x2c: return MF ? instructionBankRead8(&WDC65816::algorithmBIT8) : instructionBankRead16(&WDC65816::algorithmBIT16);
  case 0x2d: return MF ? instructionBankRead8(&WDC65816::algorithmAND8) : instructionBankRead16(&WDC65816::algorithmAND16);
  case 0x2e: return MF ? instructionBankModify8(&WDC65816::algorithmROL8) : instructionBankModify16(&WDC65816::algorithmROL16);
  case 0x2f: return MF ? instructionLongRead8(&WDC65816::algorithmAND8) : instructionLongRead16(&WDC65816::algorithmAND16);
  case 0x30: return instructionBranch(NF == 1);
  case 0x31: return MF ? instructionIndirectIndexedRead8(&WDC65816::algorithmAND8) : instructionIndirectIndexedRead16(&WDC65816::algorithmAND16);
  case 0x32: return MF ? instructionIndirectRead8(&WDC65816::algorithmAND8) : instructionIndirectRead16(&WDC65816::algorithmAND16);
  case 0x33: return MF ? instructionIndirectStackRead8(&WDC65816::algorithmAND8) : instructionIndirectStackRead16(&WDC65816::algorithmAND16);
  case 0x34: return MF ? instructionDirectRead8(&WDC65816::algorithmBIT8, X) : instructionDirectRead16(&WDC65816::algorithmBIT16, X);
  case 0x35: return MF ? instructionDirectRead8(&WDC65816::algorithmAND8, X) : instructionDirectRead16(&WDC65816::algorithmAND16, X);
  case 0x36: return MF ? instructionDirectIndexedModify8(&WDC65816::algorithmROL8) : instructionDirectIndexedModify16(&WDC65816::algorithmROL16);
  case 0x37: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmAND8, Y) : instructionIndirectLongRead16(&WDC65816::algorithmAND16, Y);
  case 0x38: return instructionSetFlag(CF);
  case 0x39: return MF ? instructionBankRead8(&WDC65816::algorithmAND8, Y) : instructionBankRead16(&WDC65816::algorithmAND16, Y);
  case 0x3a: return MF ? instructionImpliedModify8(&WDC65816::algorithmDEC8, A) : instructionImpliedModify16(&WDC65816::algorithmDEC16, A);
  case 0x3b: return instructionTransfer16(S, A);
  case 0x3c: return MF ? instructionBankRead8(&WDC65816::algorithmBIT8, X) : instructionBankRead16(&WDC65816::algorithmBIT16, X);
  case 0x3d: return MF ? instructionBankRead8(&WDC65816::algorithmAND8, X) : instructionBankRead16(&WDC65816::algorithmAND16, X);
  case 0x3e: return MF ? instructionBankIndexedModify8(&WDC65816::algorithmROL8) : instructionBankIndexedModify16(&WDC65816::algorithmROL16);
  case 0x3f: return MF ? instructionLongRead8(&WDC65816::algorithmAND8, X) : instructionLongRead16(&WDC65816::algorithmAND16, X);
  case 0x40: return instructionReturnInterrupt();
  case 0x41: return MF ? instructionIndexedIndirectRead8(&WDC65816::algorithmEOR8) : instructionIndexedIndirectRead16(&WDC65816::algorithmEOR16);
  case 0x42: return instructionPrefix();
  case 0x43: return MF ? instructionStackRead8(&WDC65816::algorithmEOR8) : instructionStackRead16(&WDC65816::algorithmEOR16);
  opX(0x44, BlockMove, -1)
  case 0x45: return MF ? instructionDirectRead8(&WDC65816::algorithmEOR8) : instructionDirectRead16(&WDC65816::algorithmEOR16);
  case 0x46: return MF ? instructionDirectModify8(&WDC65816::algorithmLSR8) : instructionDirectModify16(&WDC65816::algorithmLSR16);
  case 0x47: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmEOR8) : instructionIndirectLongRead16(&WDC65816::algorithmEOR16);
  case 0x48: return MF ? instructionPush8(A) : instructionPush16(A);
  case 0x49: return MF ? instructionImmediateRead8(&WDC65816::algorithmEOR8) : instructionImmediateRead16(&WDC65816::algorithmEOR16);
  case 0x4a: return MF ? instructionImpliedModify8(&WDC65816::algorithmLSR8, A) : instructionImpliedModify16(&WDC65816::algorithmLSR16, A);
  case 0x4b: return instructionPush8((r16)PC.r24_lsb4.b);
  case 0x4c: return instructionJumpShort();
  case 0x4d: return MF ? instructionBankRead8(&WDC65816::algorithmEOR8) : instructionBankRead16(&WDC65816::algorithmEOR16);
  case 0x4e: return MF ? instructionBankModify8(&WDC65816::algorithmLSR8) : instructionBankModify16(&WDC65816::algorithmLSR16);
  case 0x4f: return MF ? instructionLongRead8(&WDC65816::algorithmEOR8) : instructionLongRead16(&WDC65816::algorithmEOR16);
  case 0x50: return instructionBranch(VF == 0);
  case 0x51: return MF ? instructionIndirectIndexedRead8(&WDC65816::algorithmEOR8) : instructionIndirectIndexedRead16(&WDC65816::algorithmEOR16);
  case 0x52: return MF ? instructionIndirectRead8(&WDC65816::algorithmEOR8) : instructionIndirectRead16(&WDC65816::algorithmEOR16);
  case 0x53: return MF ? instructionIndirectStackRead8(&WDC65816::algorithmEOR8) : instructionIndirectStackRead16(&WDC65816::algorithmEOR16);
  opX(0x54, BlockMove, +1)
  case 0x55: return MF ? instructionDirectRead8(&WDC65816::algorithmEOR8, X) : instructionDirectRead16(&WDC65816::algorithmEOR16, X);
  case 0x56: return MF ? instructionDirectIndexedModify8(&WDC65816::algorithmLSR8) : instructionDirectIndexedModify16(&WDC65816::algorithmLSR16);
  case 0x57: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmEOR8, Y) : instructionIndirectLongRead16(&WDC65816::algorithmEOR16, Y);
  case 0x58: return instructionClearFlag(IF);
  case 0x59: return MF ? instructionBankRead8(&WDC65816::algorithmEOR8, Y) : instructionBankRead16(&WDC65816::algorithmEOR16, Y);
  opX(0x5a, Push, Y)
  case 0x5b: return instructionTransfer16(A, D);
  case 0x5c: return instructionJumpLong();
  case 0x5d: return MF ? instructionBankRead8(&WDC65816::algorithmEOR8, X) : instructionBankRead16(&WDC65816::algorithmEOR16, X);
  case 0x5e: return MF ? instructionBankIndexedModify8(&WDC65816::algorithmLSR8) : instructionBankIndexedModify16(&WDC65816::algorithmLSR16);
  case 0x5f: return MF ? instructionLongRead8(&WDC65816::algorithmEOR8, X) : instructionLongRead16(&WDC65816::algorithmEOR16, X);
  case 0x60: return instructionReturnShort();
  case 0x61: return MF ? instructionIndexedIndirectRead8(&WDC65816::algorithmADC8) : instructionIndexedIndirectRead16(&WDC65816::algorithmADC16);
  case 0x62: return instructionPushEffectiveRelativeAddress();
  case 0x63: return MF ? instructionStackRead8(&WDC65816::algorithmADC8) : instructionStackRead16(&WDC65816::algorithmADC16);
  case 0x64: return MF ? instructionDirectWrite8(Z) : instructionDirectWrite16(Z);
  case 0x65: return MF ? instructionDirectRead8(&WDC65816::algorithmADC8) : instructionDirectRead16(&WDC65816::algorithmADC16);
  case 0x66: return MF ? instructionDirectModify8(&WDC65816::algorithmROR8) : instructionDirectModify16(&WDC65816::algorithmROR16);
  case 0x67: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmADC8) : instructionIndirectLongRead16(&WDC65816::algorithmADC16);
  case 0x68: return MF ? instructionPull8(A) : instructionPull16(A);
  case 0x69: return MF ? instructionImmediateRead8(&WDC65816::algorithmADC8) : instructionImmediateRead16(&WDC65816::algorithmADC16);
  case 0x6a: return MF ? instructionImpliedModify8(&WDC65816::algorithmROR8, A) : instructionImpliedModify16(&WDC65816::algorithmROR16, A);
  case 0x6b: return instructionReturnLong();
  case 0x6c: return instructionJumpIndirect();
  case 0x6d: return MF ? instructionBankRead8(&WDC65816::algorithmADC8) : instructionBankRead16(&WDC65816::algorithmADC16);
  case 0x6e: return MF ? instructionBankModify8(&WDC65816::algorithmROR8) : instructionBankModify16(&WDC65816::algorithmROR16);
  case 0x6f: return MF ? instructionLongRead8(&WDC65816::algorithmADC8) : instructionLongRead16(&WDC65816::algorithmADC16);
  case 0x70: return instructionBranch(VF == 1);
  case 0x71: return MF ? instructionIndirectIndexedRead8(&WDC65816::algorithmADC8) : instructionIndirectIndexedRead16(&WDC65816::algorithmADC16);
  case 0x72: return MF ? instructionIndirectRead8(&WDC65816::algorithmADC8) : instructionIndirectRead16(&WDC65816::algorithmADC16);
  case 0x73: return MF ? instructionIndirectStackRead8(&WDC65816::algorithmADC8) : instructionIndirectStackRead16(&WDC65816::algorithmADC16);
  case 0x74: return MF ? instructionDirectWrite8(Z, X) : instructionDirectWrite16(Z, X);
  case 0x75: return MF ? instructionDirectRead8(&WDC65816::algorithmADC8, X) : instructionDirectRead16(&WDC65816::algorithmADC16, X);
  case 0x76: return MF ? instructionDirectIndexedModify8(&WDC65816::algorithmROR8) : instructionDirectIndexedModify16(&WDC65816::algorithmROR16);
  case 0x77: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmADC8, Y) : instructionIndirectLongRead16(&WDC65816::algorithmADC16, Y);
  case 0x78: return instructionSetFlag(IF);
  case 0x79: return MF ? instructionBankRead8(&WDC65816::algorithmADC8, Y) : instructionBankRead16(&WDC65816::algorithmADC16, Y);
  opX(0x7a, Pull, Y)
  case 0x7b: return instructionTransfer16(D, A);
  case 0x7c: return instructionJumpIndexedIndirect();
  case 0x7d: return MF ? instructionBankRead8(&WDC65816::algorithmADC8, X) : instructionBankRead16(&WDC65816::algorithmADC16, X);
  case 0x7e: return MF ? instructionBankIndexedModify8(&WDC65816::algorithmROR8) : instructionBankIndexedModify16(&WDC65816::algorithmROR16);
  case 0x7f: return MF ? instructionLongRead8(&WDC65816::algorithmADC8, X) : instructionLongRead16(&WDC65816::algorithmADC16, X);
  case 0x80: return instructionBranch();
  case 0x81: return MF ? instructionIndexedIndirectWrite8() : instructionIndexedIndirectWrite16();
  case 0x82: return instructionBranchLong();
  case 0x83: return MF ? instructionStackWrite8() : instructionStackWrite16();
  opX(0x84, DirectWrite, Y)
  case 0x85: return MF ? instructionDirectWrite8(A) : instructionDirectWrite16(A);
  opX(0x86, DirectWrite, X)
  case 0x87: return MF ? instructionIndirectLongWrite8() : instructionIndirectLongWrite16();
  opX(0x88, ImpliedModify, x(DEC), Y)
  case 0x89: return MF ? instructionBitImmediate8() : instructionBitImmediate16();
  case 0x8a: return MF ? instructionTransfer8(X, A) : instructionTransfer16(X, A);
  case 0x8b: return instructionPush8((r16)B);
  opX(0x8c, BankWrite, Y)
  case 0x8d: return MF ? instructionBankWrite8(A) : instructionBankWrite16(A);
  opX(0x8e, BankWrite, X)
  case 0x8f: return MF ? instructionLongWrite8() : instructionLongWrite16();
  case 0x90: return instructionBranch(CF == 0);
  case 0x91: return MF ? instructionIndirectIndexedWrite8() : instructionIndirectIndexedWrite16();
  case 0x92: return MF ? instructionIndirectWrite8() : instructionIndirectWrite16();
  case 0x93: return MF ? instructionIndirectStackWrite8() : instructionIndirectStackWrite16();
  opX(0x94, DirectWrite, Y, X)
  case 0x95: return MF ? instructionDirectWrite8(A, X) : instructionDirectWrite16(A, X);
  opX(0x96, DirectWrite, X, Y)
  case 0x97: return MF ? instructionIndirectLongWrite8(Y) : instructionIndirectLongWrite16(Y);
  case 0x98: return MF ? instructionTransfer8(Y, A) : instructionTransfer16(Y, A);
  case 0x99: return MF ? instructionBankWrite8(A, Y) : instructionBankWrite16(A, Y);
  case 0x9a: return instructionTransferXS();
  opX(0x9b, Transfer, X, Y)
  case 0x9c: return MF ? instructionBankWrite8(Z) : instructionBankWrite16(Z);
  case 0x9d: return MF ? instructionBankWrite8(A, X) : instructionBankWrite16(A, X);
  case 0x9e: return MF ? instructionBankWrite8(Z, X) : instructionBankWrite16(Z, X);
  case 0x9f: return MF ? instructionLongWrite8(X) : instructionLongWrite16(X);
  opX(0xa0, ImmediateRead, x(LDY))
  case 0xa1: return MF ? instructionIndexedIndirectRead8(&WDC65816::algorithmLDA8) : instructionIndexedIndirectRead16(&WDC65816::algorithmLDA16);
  opX(0xa2, ImmediateRead, x(LDX))
  case 0xa3: return MF ? instructionStackRead8(&WDC65816::algorithmLDA8) : instructionStackRead16(&WDC65816::algorithmLDA16);
  opX(0xa4, DirectRead, x(LDY))
  case 0xa5: return MF ? instructionDirectRead8(&WDC65816::algorithmLDA8) : instructionDirectRead16(&WDC65816::algorithmLDA16);
  opX(0xa6, DirectRead, x(LDX))
  case 0xa7: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmLDA8) : instructionIndirectLongRead16(&WDC65816::algorithmLDA16);
  opX(0xa8, Transfer, A, Y)
  case 0xa9: return MF ? instructionImmediateRead8(&WDC65816::algorithmLDA8) : instructionImmediateRead16(&WDC65816::algorithmLDA16);
  opX(0xaa, Transfer, A, X)
  case 0xab: return instructionPullB();
  opX(0xac, BankRead, x(LDY))
  case 0xad: return MF ? instructionBankRead8(&WDC65816::algorithmLDA8) : instructionBankRead16(&WDC65816::algorithmLDA16);
  opX(0xae, BankRead, x(LDX))
  case 0xaf: return MF ? instructionLongRead8(&WDC65816::algorithmLDA8) : instructionLongRead16(&WDC65816::algorithmLDA16);
  case 0xb0: return instructionBranch(CF == 1);
  case 0xb1: return MF ? instructionIndirectIndexedRead8(&WDC65816::algorithmLDA8) : instructionIndirectIndexedRead16(&WDC65816::algorithmLDA16);
  case 0xb2: return MF ? instructionIndirectRead8(&WDC65816::algorithmLDA8) : instructionIndirectRead16(&WDC65816::algorithmLDA16);
  case 0xb3: return MF ? instructionIndirectStackRead8(&WDC65816::algorithmLDA8) : instructionIndirectStackRead16(&WDC65816::algorithmLDA16);
  opX(0xb4, DirectRead, x(LDY), X)
  case 0xb5: return MF ? instructionDirectRead8(&WDC65816::algorithmLDA8, X) : instructionDirectRead16(&WDC65816::algorithmLDA16, X);
  opX(0xb6, DirectRead, x(LDX), Y)
  case 0xb7: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmLDA8, Y) : instructionIndirectLongRead16(&WDC65816::algorithmLDA16, Y);
  case 0xb8: return instructionClearFlag(VF);
  case 0xb9: return MF ? instructionBankRead8(&WDC65816::algorithmLDA8, Y) : instructionBankRead16(&WDC65816::algorithmLDA16, Y);
  opX(0xba, TransferSX)
  opX(0xbb, Transfer, Y, X)
  opX(0xbc, BankRead, x(LDY), X)
  case 0xbd: return MF ? instructionBankRead8(&WDC65816::algorithmLDA8, X) : instructionBankRead16(&WDC65816::algorithmLDA16, X);
  opX(0xbe, BankRead, x(LDX), Y)
  case 0xbf: return MF ? instructionLongRead8(&WDC65816::algorithmLDA8, X) : instructionLongRead16(&WDC65816::algorithmLDA16, X);
  opX(0xc0, ImmediateRead, x(CPY))
  case 0xc1: return MF ? instructionIndexedIndirectRead8(&WDC65816::algorithmCMP8) : instructionIndexedIndirectRead16(&WDC65816::algorithmCMP16);
  case 0xc2: return instructionResetP();
  case 0xc3: return MF ? instructionStackRead8(&WDC65816::algorithmCMP8) : instructionStackRead16(&WDC65816::algorithmCMP16);
  opX(0xc4, DirectRead, x(CPY))
  case 0xc5: return MF ? instructionDirectRead8(&WDC65816::algorithmCMP8) : instructionDirectRead16(&WDC65816::algorithmCMP16);
  case 0xc6: return MF ? instructionDirectModify8(&WDC65816::algorithmDEC8) : instructionDirectModify16(&WDC65816::algorithmDEC16);
  case 0xc7: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmCMP8) : instructionIndirectLongRead16(&WDC65816::algorithmCMP16);
  opX(0xc8, ImpliedModify, x(INC), Y)
  case 0xc9: return MF ? instructionImmediateRead8(&WDC65816::algorithmCMP8) : instructionImmediateRead16(&WDC65816::algorithmCMP16);
  opX(0xca, ImpliedModify, x(DEC), X)
  case 0xcb: return instructionWait();
  opX(0xcc, BankRead, x(CPY))
  case 0xcd: return MF ? instructionBankRead8(&WDC65816::algorithmCMP8) : instructionBankRead16(&WDC65816::algorithmCMP16);
  case 0xce: return MF ? instructionBankModify8(&WDC65816::algorithmDEC8) : instructionBankModify16(&WDC65816::algorithmDEC16);
  case 0xcf: return MF ? instructionLongRead8(&WDC65816::algorithmCMP8) : instructionLongRead16(&WDC65816::algorithmCMP16);
  case 0xd0: return instructionBranch(ZF == 0);
  case 0xd1: return MF ? instructionIndirectIndexedRead8(&WDC65816::algorithmCMP8) : instructionIndirectIndexedRead16(&WDC65816::algorithmCMP16);
  case 0xd2: return MF ? instructionIndirectRead8(&WDC65816::algorithmCMP8) : instructionIndirectRead16(&WDC65816::algorithmCMP16);
  case 0xd3: return MF ? instructionIndirectStackRead8(&WDC65816::algorithmCMP8) : instructionIndirectStackRead16(&WDC65816::algorithmCMP16);
  case 0xd4: return instructionPushEffectiveIndirectAddress();
  case 0xd5: return MF ? instructionDirectRead8(&WDC65816::algorithmCMP8, X) : instructionDirectRead16(&WDC65816::algorithmCMP16, X);
  case 0xd6: return MF ? instructionDirectIndexedModify8(&WDC65816::algorithmDEC8) : instructionDirectIndexedModify16(&WDC65816::algorithmDEC16);
  case 0xd7: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmCMP8, Y) : instructionIndirectLongRead16(&WDC65816::algorithmCMP16, Y);
  case 0xd8: return instructionClearFlag(DF);
  case 0xd9: return MF ? instructionBankRead8(&WDC65816::algorithmCMP8, Y) : instructionBankRead16(&WDC65816::algorithmCMP16, Y);
  opX(0xda, Push, X)
  case 0xdb: return instructionStop();
  case 0xdc: return instructionJumpIndirectLong();
  case 0xdd: return MF ? instructionBankRead8(&WDC65816::algorithmCMP8, X) : instructionBankRead16(&WDC65816::algorithmCMP16, X);
  case 0xde: return MF ? instructionBankIndexedModify8(&WDC65816::algorithmDEC8) : instructionBankIndexedModify16(&WDC65816::algorithmDEC16);
  case 0xdf: return MF ? instructionLongRead8(&WDC65816::algorithmCMP8, X) : instructionLongRead16(&WDC65816::algorithmCMP16, X);
  opX(0xe0, ImmediateRead, x(CPX))
  case 0xe1: return MF ? instructionIndexedIndirectRead8(&WDC65816::algorithmSBC8) : instructionIndexedIndirectRead16(&WDC65816::algorithmSBC16);
  case 0xe2: return instructionSetP();
  case 0xe3: return MF ? instructionStackRead8(&WDC65816::algorithmSBC8) : instructionStackRead16(&WDC65816::algorithmSBC16);
  opX(0xe4, DirectRead, x(CPX))
  case 0xe5: return MF ? instructionDirectRead8(&WDC65816::algorithmSBC8) : instructionDirectRead16(&WDC65816::algorithmSBC16);
  case 0xe6: return MF ? instructionDirectModify8(&WDC65816::algorithmINC8) : instructionDirectModify16(&WDC65816::algorithmINC16);
  case 0xe7: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmSBC8) : instructionIndirectLongRead16(&WDC65816::algorithmSBC16);
  opX(0xe8, ImpliedModify, x(INC), X)
  case 0xe9: return MF ? instructionImmediateRead8(&WDC65816::algorithmSBC8) : instructionImmediateRead16(&WDC65816::algorithmSBC16);
  case 0xea: return instructionNoOperation();
  case 0xeb: return instructionExchangeBA();
  opX(0xec, BankRead, x(CPX))
  case 0xed: return MF ? instructionBankRead8(&WDC65816::algorithmSBC8) : instructionBankRead16(&WDC65816::algorithmSBC16);
  case 0xee: return MF ? instructionBankModify8(&WDC65816::algorithmINC8) : instructionBankModify16(&WDC65816::algorithmINC16);
  case 0xef: return MF ? instructionLongRead8(&WDC65816::algorithmSBC8) : instructionLongRead16(&WDC65816::algorithmSBC16);
  case 0xf0: return instructionBranch(ZF == 1);
  case 0xf1: return MF ? instructionIndirectIndexedRead8(&WDC65816::algorithmSBC8) : instructionIndirectIndexedRead16(&WDC65816::algorithmSBC16);
  case 0xf2: return MF ? instructionIndirectRead8(&WDC65816::algorithmSBC8) : instructionIndirectRead16(&WDC65816::algorithmSBC16);
  case 0xf3: return MF ? instructionIndirectStackRead8(&WDC65816::algorithmSBC8) : instructionIndirectStackRead16(&WDC65816::algorithmSBC16);
  case 0xf4: return instructionPushEffectiveAddress();
  case 0xf5: return MF ? instructionDirectRead8(&WDC65816::algorithmSBC8, X) : instructionDirectRead16(&WDC65816::algorithmSBC16, X);
  case 0xf6: return MF ? instructionDirectIndexedModify8(&WDC65816::algorithmINC8) : instructionDirectIndexedModify16(&WDC65816::algorithmINC16);
  case 0xf7: return MF ? instructionIndirectLongRead8(&WDC65816::algorithmSBC8, Y) : instructionIndirectLongRead16(&WDC65816::algorithmSBC16, Y);
  case 0xf8: return instructionSetFlag(DF);
  case 0xf9: return MF ? instructionBankRead8(&WDC65816::algorithmSBC8, Y) : instructionBankRead16(&WDC65816::algorithmSBC16, Y);
  opX(0xfa, Pull, X)
  case 0xfb: return instructionExchangeCE();
  case 0xfc: return instructionCallIndexedIndirect();
  case 0xfd: return MF ? instructionBankRead8(&WDC65816::algorithmSBC8, X) : instructionBankRead16(&WDC65816::algorithmSBC16, X);
  case 0xfe: return MF ? instructionBankIndexedModify8(&WDC65816::algorithmINC8) : instructionBankIndexedModify16(&WDC65816::algorithmINC16);
  case 0xff: return MF ? instructionLongRead8(&WDC65816::algorithmSBC8, X) : instructionLongRead16(&WDC65816::algorithmSBC16, X);
}
