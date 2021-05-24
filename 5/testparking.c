//
//  testparking.c
//  
//
//  Created by 朱世耘 on 2020/1/9.
//

#include <8051.h>
#include "preemptive.h"

// output char
#define print(s){\
    SBUF= s;\
    while(TI==0){}\
    TI=0;\
}

// output integer ( consider >= 10 )
#define printint(s){\
if(s<='9'){\
    SBUF= s;\
    while(TI==0){}\
    TI=0;\
}\
else{\
x=0;\
while(s>'9'){\
s--;\
s--;\
s--;\
s--;\
s--;\
s--;\
s--;\
s--;\
s--;\
s--;\
x++;\
}\
SBUF= ('0'+x);\
   while(TI==0){}\
   TI=0;\
SBUF= s;\
   while(TI==0){}\
   TI=0;\
}\
}

__data __at (0x2E) char x;
__data __at (0x20) char mutex;
__data __at (0x7D) char printer;
__data __at (0x22) char parklot[2];
__data __at (0x31) char log[15];


// Car 1 to 5, find empty parklot, record in log[], leave
void Car1(void) {
    log[0] = now();
    SemaphoreWait(mutex);
    if (parklot[0]==0) {
        parklot[0]=1;
        log[2]=0;
    }
    else{
        parklot[1]=1;
        log[2]=1;
    }
    SemaphoreSignal(mutex);
    delay(2);
    log[1] = now();
    parklot[(log[2])]=0;
    SemaphoreSignal(printer);
    ThreadExit();
}

void Car2(void) {
    log[3] = now();
    SemaphoreWait(mutex);
    if (parklot[0]==0) {
        parklot[0]=2;
        log[5]=0;
    }
    else{
        parklot[1]=2;
        log[5]=1;
    }
    SemaphoreSignal(mutex);
    delay(3);
    log[4] = now();
    parklot[(log[5])]=0;
    SemaphoreSignal(printer);
    ThreadExit();
}

void Car3(void) {
    log[6] = now();
    SemaphoreWait(mutex);
    if (parklot[0]==0) {
        parklot[0]=3;
        log[8]=0;
    }
    else{
        parklot[1]=3;
        log[8]=1;
    }
    SemaphoreSignal(mutex);
    delay(5);
    log[7] = now();
    parklot[(log[8])]=0;
    SemaphoreSignal(printer);
    ThreadExit();
}

void Car4(void) {
    log[9] = now();
    SemaphoreWait(mutex);
    if (parklot[0]==0) {
        parklot[0]=4;
        log[11]=0;
    }
    else{
        parklot[1]=4;
        log[11]=1;
    }
    SemaphoreSignal(mutex);
    delay(3);
    log[10] = now();
    parklot[(log[11])]=0;
    SemaphoreSignal(printer);
    ThreadExit();
}

void Car5(void) {
    log[12] = now();
    SemaphoreWait(mutex);
    if (parklot[0]==0) {
        parklot[0]=5;
        log[14]=0;
    }
    else{
        parklot[1]=5;
        log[14]=1;
    }
    SemaphoreSignal(mutex);
    delay(6);
    log[13] = now();
    parklot[(log[14])]=0;
    SemaphoreSignal(printer);
    ThreadExit();
}

// output log[]
void output(){
    // wait until five cars have leaved
    SemaphoreWait(printer);
    SemaphoreWait(printer);
    SemaphoreWait(printer);
    SemaphoreWait(printer);
    SemaphoreWait(printer);
    
    TMOD |= 0x20;
    TH1 = -6;
    SCON = 0x50;
    TR1 = 1;
    
    // output car i:arrive/leave/location
    print('\0');
    print('a');
    print('r');
    print('r');
    print('i');
    print('v');
    print('e');
    print('/');
    print('l');
    print('e');
    print('a');
    print('v');
    print('e');
    print('/');
    print('s');
    print('p');
    print('a');
    print('c');
    print('e');
    print('\n');
    
    print('c');
    print('a');
    print('r');
    print('1');
    print(':');
    printint('0'+log[0]);
    print('/');
    printint('0'+log[1]);
    print('/');
    printint('0'+log[2]);
    print('\n');
    
    print('c');
    print('a');
    print('r');
    print('2');
    print(':');
    printint('0'+log[3]);
    print('/');
    printint('0'+log[4]);
    print('/');
    printint('0'+log[5]);
    print('\n');
    
    print('c');
    print('a');
    print('r');
    print('3');
    print(':');
    printint('0'+log[6]);
    print('/');
    printint('0'+log[7]);
    print('/');
    printint('0'+log[8]);
    print('\n');
    
    print('c');
    print('a');
    print('r');
    print('4');
    print(':');
    printint('0'+log[9]);
    print('/');
    printint('0'+log[10]);
    print('/');
    printint('0'+log[11]);
    print('\n');
    
    print('c');
    print('a');
    print('r');
    print('5');
    print(':');
    printint('0'+log[12]);
    print('/');
    printint('0'+log[13]);
    print('/');
    printint('0'+log[14]);
    
    ThreadExit();
}

void main(void) {
    // initialize Semaphore and parklot
    SemaphoreCreate(mutex, 1);
    SemaphoreCreate(printer, 0);
    parklot[0]=0;
    parklot[1]=0;
    
    ThreadCreate(Car1);
    ThreadCreate(Car2);
    ThreadCreate(Car3);
    ThreadCreate(Car4);
    ThreadCreate(Car5);
    output();

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
