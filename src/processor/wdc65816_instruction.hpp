switch(fetch()) {
  //emulation mode lacks BRK vector; uses IRQ vector instead
  case 0x00: return instructionInterrupt(EF ? (r16)0xfffe : (r16)0xffe6);
  opM(0x01, IndexedIndirectRead, m(ORA))
  case 0x02: return instructionInterrupt(EF ? (r16)0xfff4 : (r16)0xffe4);
  opM(0x03, StackRead, m(ORA))
  opM(0x04, DirectModify, m(TSB))
  opM(0x05, DirectRead, m(ORA))
  opM(0x06, DirectModify, m(ASL))
  opM(0x07, IndirectLongRead, m(ORA))
  case 0x08: return instructionPush8((r16)P);
  opM(0x09, ImmediateRead, m(ORA))
  opM(0x0a, ImpliedModify, m(ASL), A)
  case 0x0b: return instructionPushD();
  opM(0x0c, BankModify, m(TSB))
  opM(0x0d, BankRead, m(ORA))
  opM(0x0e, BankModify, m(ASL))
  opM(0x0f, LongRead, m(ORA))
  case 0x10: return instructionBranch(NF == 0);
  opM(0x11, IndirectIndexedRead, m(ORA))
  opM(0x12, IndirectRead, m(ORA))
  opM(0x13, IndirectStackRead, m(ORA))
  opM(0x14, DirectModify, m(TRB))
  opM(0x15, DirectRead, m(ORA), X)
  opM(0x16, DirectIndexedModify, m(ASL))
  opM(0x17, IndirectLongRead, m(ORA), Y)
  case 0x18: return instructionClearFlag(CF);
  opM(0x19, BankRead, m(ORA), Y)
  opM(0x1a, ImpliedModify, m(INC), A)
  case 0x1b: return instructionTransferCS();
  opM(0x1c, BankModify, m(TRB))
  opM(0x1d, BankRead, m(ORA), X)
  opM(0x1e, BankIndexedModify, m(ASL))
  opM(0x1f, LongRead, m(ORA), X)
  case 0x20: return instructionCallShort();
  opM(0x21, IndexedIndirectRead, m(AND))
  case 0x22: return instructionCallLong();
  opM(0x23, StackRead, m(AND))
  opM(0x24, DirectRead, m(BIT))
  opM(0x25, DirectRead, m(AND))
  opM(0x26, DirectModify, m(ROL))
  opM(0x27, IndirectLongRead, m(AND))
  case 0x28: return instructionPullP();
  opM(0x29, ImmediateRead, m(AND))
  opM(0x2a, ImpliedModify, m(ROL), A)
  case 0x2b: return instructionPullD();
  opM(0x2c, BankRead, m(BIT))
  opM(0x2d, BankRead, m(AND))
  opM(0x2e, BankModify, m(ROL))
  opM(0x2f, LongRead, m(AND))
  case 0x30: return instructionBranch(NF == 1);
  opM(0x31, IndirectIndexedRead, m(AND))
  opM(0x32, IndirectRead, m(AND))
  opM(0x33, IndirectStackRead, m(AND))
  opM(0x34, DirectRead, m(BIT), X)
  opM(0x35, DirectRead, m(AND), X)
  opM(0x36, DirectIndexedModify, m(ROL))
  opM(0x37, IndirectLongRead, m(AND), Y)
  case 0x38: return instructionSetFlag(CF);
  opM(0x39, BankRead, m(AND), Y)
  opM(0x3a, ImpliedModify, m(DEC), A)
  case 0x3b: return instructionTransfer16(S, A);
  opM(0x3c, BankRead, m(BIT), X)
  opM(0x3d, BankRead, m(AND), X)
  opM(0x3e, BankIndexedModify, m(ROL))
  opM(0x3f, LongRead, m(AND), X)
  case 0x40: return instructionReturnInterrupt();
  opM(0x41, IndexedIndirectRead, m(EOR))
  case 0x42: return instructionPrefix();
  opM(0x43, StackRead, m(EOR))
  opX(0x44, BlockMove, -1)
  opM(0x45, DirectRead, m(EOR))
  opM(0x46, DirectModify, m(LSR))
  opM(0x47, IndirectLongRead, m(EOR))
  opM(0x48, Push, A)
  opM(0x49, ImmediateRead, m(EOR))
  opM(0x4a, ImpliedModify, m(LSR), A)
  case 0x4b: return instructionPush8((r16)PC.r24_lsb4.b);
  case 0x4c: return instructionJumpShort();
  opM(0x4d, BankRead, m(EOR))
  opM(0x4e, BankModify, m(LSR))
  opM(0x4f, LongRead, m(EOR))
  case 0x50: return instructionBranch(VF == 0);
  opM(0x51, IndirectIndexedRead, m(EOR))
  opM(0x52, IndirectRead, m(EOR))
  opM(0x53, IndirectStackRead, m(EOR))
  opX(0x54, BlockMove, +1)
  opM(0x55, DirectRead, m(EOR), X)
  opM(0x56, DirectIndexedModify, m(LSR))
  opM(0x57, IndirectLongRead, m(EOR), Y)
  case 0x58: return instructionClearFlag(IF);
  opM(0x59, BankRead, m(EOR), Y)
  opX(0x5a, Push, Y)
  case 0x5b: return instructionTransfer16(A, D);
  case 0x5c: return instructionJumpLong();
  opM(0x5d, BankRead, m(EOR), X)
  opM(0x5e, BankIndexedModify, m(LSR))
  opM(0x5f, LongRead, m(EOR), X)
  case 0x60: return instructionReturnShort();
  opM(0x61, IndexedIndirectRead, m(ADC))
  case 0x62: return instructionPushEffectiveRelativeAddress();
  opM(0x63, StackRead, m(ADC))
  opM(0x64, DirectWrite, Z)
  opM(0x65, DirectRead, m(ADC))
  opM(0x66, DirectModify, m(ROR))
  opM(0x67, IndirectLongRead, m(ADC))
  opM(0x68, Pull, A)
  opM(0x69, ImmediateRead, m(ADC))
  opM(0x6a, ImpliedModify, m(ROR), A)
  case 0x6b: return instructionReturnLong();
  case 0x6c: return instructionJumpIndirect();
  opM(0x6d, BankRead, m(ADC))
  opM(0x6e, BankModify, m(ROR))
  opM(0x6f, LongRead, m(ADC))
  case 0x70: return instructionBranch(VF == 1);
  opM(0x71, IndirectIndexedRead, m(ADC))
  opM(0x72, IndirectRead, m(ADC))
  opM(0x73, IndirectStackRead, m(ADC))
  opM(0x74, DirectWrite, Z, X)
  opM(0x75, DirectRead, m(ADC), X)
  opM(0x76, DirectIndexedModify, m(ROR))
  opM(0x77, IndirectLongRead, m(ADC), Y)
  case 0x78: return instructionSetFlag(IF);
  opM(0x79, BankRead, m(ADC), Y)
  opX(0x7a, Pull, Y)
  case 0x7b: return instructionTransfer16(D, A);
  case 0x7c: return instructionJumpIndexedIndirect();
  opM(0x7d, BankRead, m(ADC), X)
  opM(0x7e, BankIndexedModify, m(ROR))
  opM(0x7f, LongRead, m(ADC), X)
  case 0x80: return instructionBranch();
  opM(0x81, IndexedIndirectWrite)
  case 0x82: return instructionBranchLong();
  opM(0x83, StackWrite)
  opX(0x84, DirectWrite, Y)
  opM(0x85, DirectWrite, A)
  opX(0x86, DirectWrite, X)
  opM(0x87, IndirectLongWrite)
  opX(0x88, ImpliedModify, x(DEC), Y)
  opM(0x89, BitImmediate)
  opM(0x8a, Transfer, X, A)
  case 0x8b: return instructionPush8((r16)B);
  opX(0x8c, BankWrite, Y)
  opM(0x8d, BankWrite, A)
  opX(0x8e, BankWrite, X)
  opM(0x8f, LongWrite)
  case 0x90: return instructionBranch(CF == 0);
  opM(0x91, IndirectIndexedWrite)
  opM(0x92, IndirectWrite)
  opM(0x93, IndirectStackWrite)
  opX(0x94, DirectWrite, Y, X)
  opM(0x95, DirectWrite, A, X)
  opX(0x96, DirectWrite, X, Y)
  opM(0x97, IndirectLongWrite, Y)
  opM(0x98, Transfer, Y, A)
  opM(0x99, BankWrite, A, Y)
  case 0x9a: return instructionTransferXS();
  opX(0x9b, Transfer, X, Y)
  opM(0x9c, BankWrite, Z)
  opM(0x9d, BankWrite, A, X)
  opM(0x9e, BankWrite, Z, X)
  opM(0x9f, LongWrite, X)
  opX(0xa0, ImmediateRead, x(LDY))
  opM(0xa1, IndexedIndirectRead, m(LDA))
  opX(0xa2, ImmediateRead, x(LDX))
  opM(0xa3, StackRead, m(LDA))
  opX(0xa4, DirectRead, x(LDY))
  opM(0xa5, DirectRead, m(LDA))
  opX(0xa6, DirectRead, x(LDX))
  opM(0xa7, IndirectLongRead, m(LDA))
  opX(0xa8, Transfer, A, Y)
  opM(0xa9, ImmediateRead, m(LDA))
  opX(0xaa, Transfer, A, X)
  case 0xab: return instructionPullB();
  opX(0xac, BankRead, x(LDY))
  opM(0xad, BankRead, m(LDA))
  opX(0xae, BankRead, x(LDX))
  opM(0xaf, LongRead, m(LDA))
  case 0xb0: return instructionBranch(CF == 1);
  opM(0xb1, IndirectIndexedRead, m(LDA))
  opM(0xb2, IndirectRead, m(LDA))
  opM(0xb3, IndirectStackRead, m(LDA))
  opX(0xb4, DirectRead, x(LDY), X)
  opM(0xb5, DirectRead, m(LDA), X)
  opX(0xb6, DirectRead, x(LDX), Y)
  opM(0xb7, IndirectLongRead, m(LDA), Y)
  case 0xb8: return instructionClearFlag(VF);
  opM(0xb9, BankRead, m(LDA), Y)
  opX(0xba, TransferSX)
  opX(0xbb, Transfer, Y, X)
  opX(0xbc, BankRead, x(LDY), X)
  opM(0xbd, BankRead, m(LDA), X)
  opX(0xbe, BankRead, x(LDX), Y)
  opM(0xbf, LongRead, m(LDA), X)
  opX(0xc0, ImmediateRead, x(CPY))
  opM(0xc1, IndexedIndirectRead, m(CMP))
  case 0xc2: return instructionResetP();
  opM(0xc3, StackRead, m(CMP))
  opX(0xc4, DirectRead, x(CPY))
  opM(0xc5, DirectRead, m(CMP))
  opM(0xc6, DirectModify, m(DEC))
  opM(0xc7, IndirectLongRead, m(CMP))
  opX(0xc8, ImpliedModify, x(INC), Y)
  opM(0xc9, ImmediateRead, m(CMP))
  opX(0xca, ImpliedModify, x(DEC), X)
  case 0xcb: return instructionWait();
  opX(0xcc, BankRead, x(CPY))
  opM(0xcd, BankRead, m(CMP))
  opM(0xce, BankModify, m(DEC))
  opM(0xcf, LongRead, m(CMP))
  case 0xd0: return instructionBranch(ZF == 0);
  opM(0xd1, IndirectIndexedRead, m(CMP))
  opM(0xd2, IndirectRead, m(CMP))
  opM(0xd3, IndirectStackRead, m(CMP))
  case 0xd4: return instructionPushEffectiveIndirectAddress();
  opM(0xd5, DirectRead, m(CMP), X)
  opM(0xd6, DirectIndexedModify, m(DEC))
  opM(0xd7, IndirectLongRead, m(CMP), Y)
  case 0xd8: return instructionClearFlag(DF);
  opM(0xd9, BankRead, m(CMP), Y)
  opX(0xda, Push, X)
  case 0xdb: return instructionStop();
  case 0xdc: return instructionJumpIndirectLong();
  opM(0xdd, BankRead, m(CMP), X)
  opM(0xde, BankIndexedModify, m(DEC))
  opM(0xdf, LongRead, m(CMP), X)
  opX(0xe0, ImmediateRead, x(CPX))
  opM(0xe1, IndexedIndirectRead, m(SBC))
  case 0xe2: return instructionSetP();
  opM(0xe3, StackRead, m(SBC))
  opX(0xe4, DirectRead, x(CPX))
  opM(0xe5, DirectRead, m(SBC))
  opM(0xe6, DirectModify, m(INC))
  opM(0xe7, IndirectLongRead, m(SBC))
  opX(0xe8, ImpliedModify, x(INC), X)
  opM(0xe9, ImmediateRead, m(SBC))
  case 0xea: return instructionNoOperation();
  case 0xeb: return instructionExchangeBA();
  opX(0xec, BankRead, x(CPX))
  opM(0xed, BankRead, m(SBC))
  opM(0xee, BankModify, m(INC))
  opM(0xef, LongRead, m(SBC))
  case 0xf0: return instructionBranch(ZF == 1);
  opM(0xf1, IndirectIndexedRead, m(SBC))
  opM(0xf2, IndirectRead, m(SBC))
  opM(0xf3, IndirectStackRead, m(SBC))
  case 0xf4: return instructionPushEffectiveAddress();
  opM(0xf5, DirectRead, m(SBC), X)
  opM(0xf6, DirectIndexedModify, m(INC))
  opM(0xf7, IndirectLongRead, m(SBC), Y)
  case 0xf8: return instructionSetFlag(DF);
  opM(0xf9, BankRead, m(SBC), Y)
  opX(0xfa, Pull, X)
  case 0xfb: return instructionExchangeCE();
  case 0xfc: return instructionCallIndexedIndirect();
  opM(0xfd, BankRead, m(SBC), X)
  opM(0xfe, BankIndexedModify, m(INC))
  opM(0xff, LongRead, m(SBC), X)
}
