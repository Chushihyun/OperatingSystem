//
//  testpreempt.c
//  
//
//  Created by 朱世耘 on 2020/1/9.
//

#include <8051.h>
#include "preemptive.h"

__data __at (0x30) char buffer[3];
__data __at (0x33) char c;
__data __at (0x34) char x;
__data __at (0x35) char mutex;
__data __at (0x36) char full;
__data __at (0x37) char empty;
__data __at (0x38) char buf_head;
__data __at (0x39) char buf_tail;


//  put into buffer[buf_tail]; buf_tail++ ;
void Producer(void) {
    c = 'A';
    while (1) {
         // critical section in
         SemaphoreWait(empty);
         SemaphoreWait(mutex);
         buffer[buf_tail] = c;
         // critical section out
         SemaphoreSignal(mutex);
         SemaphoreSignal(full);
        
         if (buf_tail==2) {
            buf_tail=0;
         }
         else{
            buf_tail++;
         }
         if(c=='Z') {
            c = 'A';
         }
         else c++;
    }
}

// take from buffer[buf_head]; buf_head++ ;
void Consumer(void) {
        TMOD |= 0x20;
        TH1 = -6;
        SCON = 0x50;
        TR1 = 1;
        while (1) {
            // critical in
            SemaphoreWait(full);
            SemaphoreWait(mutex);
            SBUF = buffer[buf_head];
            // critical out
            SemaphoreSignal(mutex);
            SemaphoreSignal(empty);
            
            if (buf_head==2) {
               buf_head=0;
            }
            else{
               buf_head++;
            }
            while (!TI) { }
            TI = 0;
        }
}

void main(void) {
    for (x=0; x<3; x++) {
        buffer[x]=0;
    }
    buf_head=0;
    buf_tail=0;
    SemaphoreCreate(mutex, 1);
    SemaphoreCreate(full, 0);
    SemaphoreCreate(empty, 3);
    
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
