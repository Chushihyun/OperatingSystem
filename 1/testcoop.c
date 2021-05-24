
/*
 * file: testcoop.c
 */
#include <8051.h>
#include "cooperative.h"

__data __at (0x30) char buffer;
__data __at (0x31) char c;


void Producer(void) {
    c = 'A';
    while (1) {
         if(buffer!=0){
             ThreadYield();
         }
         buffer = c;
         if(c=='Z'){
             c = 'A';
         }
         else c++;
    }
}

/* [10 pts for this function]
 * the consumer in this test program gets the next item from
 * the queue and consume it and writes it to the serial port.
 * The Consumer also does not return.
 */
void Consumer(void) {
        /* @@@ [2 pt] initialize Tx for polling */
        TMOD = 0x20;
        TH1 = -6;
        SCON = 0x50;
        TR1 = 1;
        while (1) {
                /* @@@ [2 pt] wait for new data from producer
                 * @@@ [6 pt] write data to serial port Tx,
                 * poll for Tx to finish writing (TI),
                 * then clear the flag
                 */
            if(buffer==0)    ThreadYield();
            SBUF = buffer;
            buffer = 0;
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


