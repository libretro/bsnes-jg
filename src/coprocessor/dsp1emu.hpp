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

// DSP-1's emulation code
//
// Based on research by Overload, The Dumper, Neviksti and Andreas Naive
// Date: June 2006

#ifndef __DSP1EMUL_H
#define __DSP1EMUL_H

#define DSP1_VERSION 0x0102

namespace SuperFamicom {

class Dsp1
{
   public:
      // The DSP-1 status register has 16 bits, but only
      // the upper 8 bits can be accessed from an external device, so all these
      // positions are referred to the upper byte (bits D8 to D15)
      enum SrFlags {DRC=0x04, DRS=0x10, RQM=0x80};

      // According to Overload's docs, these are the meanings of the flags:
      // DRC: The Data Register Control (DRC) bit specifies the data transfer length to and from the host CPU.
      //   0: Data transfer to and from the DSP-1 is 16 bits.
      //   1: Data transfer to and from the DSP-1 is 8 bits.
      // DRS: The Data Register Status (DRS) bit indicates the data transfer status in the case of transfering 16-bit data.
      //   0: Data transfer has terminated.
      //   1: Data transfer in progress.
      // RQM: The Request for Master (RQM) indicates that the DSP1 is requesting host CPU for data read/write.
      //   0: Internal Data Register Transfer.
      //   1: External Data Register Transfer.

      Dsp1();
      uint8_t getSr();            // return the status register's high byte
      uint8_t getDr();
      void setDr(uint8_t);
      void reset();

      void serialize(serializer&);

   private:
      enum FsmMajorState {WAIT_COMMAND, READ_DATA, WRITE_DATA};
      enum MaxDataAccesses {MAX_READS=7, MAX_WRITES=1024};

      struct Command {
         void (Dsp1::*callback)(int16_t*, int16_t*);
         unsigned int reads;
         unsigned int writes;
      };

      static const Command mCommandTable[];

      struct SharedData { // some RAM variables shared between commands
         int16_t MatrixA[3][3];          // attitude matrix A
         int16_t MatrixB[3][3];
         int16_t MatrixC[3][3];
         int16_t CentreX, CentreY, CentreZ;   // center of projection
         int16_t CentreZ_C, CentreZ_E;
         int16_t VOffset;                     // vertical offset of the screen with regard to the centre of projection
         int16_t Les, C_Les, E_Les;
         int16_t SinAas, CosAas;
         int16_t SinAzs, CosAzs;
         int16_t SinAZS, CosAZS;
         int16_t SecAZS_C1, SecAZS_E1;
         int16_t SecAZS_C2, SecAZS_E2;
         int16_t Nx, Ny, Nz;    // normal vector to the screen (norm 1, points toward the center of projection)
         int16_t Gx, Gy, Gz;    // center of the screen (global coordinates)
         int16_t Hx, Hy;        // horizontal vector of the screen (Hz=0, norm 1, points toward the right of the screen)
         int16_t Vx, Vy, Vz;    // vertical vector of the screen (norm 1, points toward the top of the screen)

      } shared;

      uint8_t mSr;                    // status register
      int mSrLowByteAccess;
      uint16_t mDr;                   // "internal" representation of the data register
      unsigned mFsmMajorState;        // current major state of the FSM
      uint8_t mCommand;               // current command processed by the FSM
      uint8_t mDataCounter;           // #uint16_t read/writes counter used by the FSM
      int16_t mReadBuffer[MAX_READS];
      int16_t mWriteBuffer[MAX_WRITES];
      bool mFreeze;                   // need explanation?  ;)

      void fsmStep(bool, uint8_t&);   // FSM logic

      // commands
      void memoryTest(int16_t*, int16_t*);
      void memoryDump(int16_t*, int16_t*);
      void memorySize(int16_t*, int16_t*);
      void multiply(int16_t*, int16_t*);
      void multiply2(int16_t*, int16_t*);
      void inverse(int16_t*, int16_t*);
      void triangle(int16_t*, int16_t*);
      void radius(int16_t*, int16_t*);
      void range(int16_t*, int16_t*);
      void range2(int16_t*, int16_t*);
      void distance(int16_t*, int16_t*);
      void rotate(int16_t*, int16_t*);
      void polar(int16_t*, int16_t*);
      void attitudeA(int16_t*, int16_t*);
      void attitudeB(int16_t*, int16_t*);
      void attitudeC(int16_t*, int16_t*);
      void objectiveA(int16_t*, int16_t*);
      void objectiveB(int16_t*, int16_t*);
      void objectiveC(int16_t*, int16_t*);
      void subjectiveA(int16_t*, int16_t*);
      void subjectiveB(int16_t*, int16_t*);
      void subjectiveC(int16_t*, int16_t*);
      void scalarA(int16_t*, int16_t*);
      void scalarB(int16_t*, int16_t*);
      void scalarC(int16_t*, int16_t*);
      void gyrate(int16_t*, int16_t*);
      void parameter(int16_t*, int16_t*);
      void raster(int16_t*, int16_t*);
      void target(int16_t*, int16_t*);
      void project(int16_t*, int16_t*);

      // auxiliar functions
      int16_t sin(int16_t);
      int16_t cos(int16_t);
      void inverse(int16_t, int16_t, int16_t&, int16_t&);
      int16_t denormalizeAndClip(int16_t, int16_t);
      void normalize(int16_t, int16_t&, int16_t&);
      void normalizeDouble(int32_t, int16_t&, int16_t&);
      int16_t shiftR(int16_t, int16_t);
};

}

#endif
