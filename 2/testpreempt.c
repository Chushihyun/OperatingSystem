//
//  testpreempt.c
//  
//
//  Created by 朱世耘 on 2020/1/9.
//

#include <8051.h>
#include "preemptive.h"

__data __at (0x30) char buffer;
__data __at (0x31) char c;

void Producer(void) {
    c = 'A';
    while (1) {
         while(buffer!=0) {}
         EA=0;
         buffer= c;
         EA=1;
         if(c=='Z') {
            c = 'A';
         }
         else c++;
    }
}

void Consumer(void) {
        TMOD |= 0x20;
        TH1 = -6;
        SCON = 0x50;
        TR1 = 1;
        while (1) {
            while(buffer==0) {}
            EA=0;
            SBUF = buffer;
            buffer = 0;
            EA=1;
            while (!TI) { }
            TI = 0;
        }
}

void main(void) {
    buffer=0;
    
    ThreadCreate(Producer);
    ThreadCreate(Consumer);
    ThreadExit();
}

void _sdcc_gsinit_startup(void) {
        __asm
                ljmp  _Bootstrap
        __endasm;
}

void _mcs51_genRAMCLEAR(void) {}
void _mcs51_genXINIT(void) {}
void _mcs51_genXRAMCLEAR(void) {}

void timer0_ISR(void) __interrupt(1) {
        __asm
                ljmp _myTimer0Handler
        __endasm;
}
