/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2022 Rupert Carmichael
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

#pragma once

namespace SuperFamicom {

struct Cx4 {
  void power();

  uint8_t read(unsigned, uint8_t = 0);
  void write(unsigned, uint8_t);

  void serialize(serializer&);

  uint8_t ram[0x0c00];
  uint8_t reg[0x0100];
  uint32_t r0, r1, r2,  r3,  r4,  r5,  r6,  r7,
           r8, r9, r10, r11, r12, r13, r14, r15;

  int16_t C4WFXVal, C4WFYVal, C4WFZVal, C4WFX2Val, C4WFY2Val, C4WFDist, C4WFScale;
  int16_t C41FXVal, C41FYVal, C41FAngleRes, C41FDist, C41FDistVal;

  void C4TransfWireFrame();
  void C4TransfWireFrame2();
  void C4CalcWireFrame();
  void C4DrawLine(int32_t, int32_t, int16_t, int32_t, int32_t, int16_t, uint8_t);
  void C4DrawWireFrame();
  void C4DoScaleRotate(int);

public:
  uint32_t ldr(uint8_t);
  uint32_t sin(uint32_t);
  uint32_t cos(uint32_t);

  void str(uint8_t, uint32_t);
  void mul(uint32_t, uint32_t, uint32_t&, uint32_t&);

  void transfer_data();
  void immediate_reg(uint32_t);

  void op00_00();
  void op00_03();
  void op00_05();
  void op00_07();
  void op00_08();
  void op00_0b();
  void op00_0c();

  void op00();
  void op01();
  void op05();
  void op0d();
  void op10();
  void op13();
  void op15();
  void op1f();
  void op22();
  void op25();
  void op2d();
  void op40();
  void op54();
  void op5c();
  void op5e();
  void op60();
  void op62();
  void op64();
  void op66();
  void op68();
  void op6a();
  void op6c();
  void op6e();
  void op70();
  void op72();
  void op74();
  void op76();
  void op78();
  void op7a();
  void op7c();
  void op89();

  uint8_t readb(uint16_t);
  uint16_t readw(uint16_t);
  uint32_t readl(uint16_t);

  void writeb(uint16_t, uint8_t);
  void writew(uint16_t, uint16_t);
  void writel(uint16_t, uint32_t);
};

extern Cx4 cx4;

}
