#include <8051.h>
#include "preemptive.h"
__data __at (0x20) char ssp[MAXTHREADS];
__data __at (0x24) char bitmap[MAXTHREADS];
__data __at (0x28) char ID;
__data __at (0x29) char manager_ID;
__data __at (0x2A) char tmp1;
__data __at (0x2B) char tmp2;
__data __at (0x2C) char i;

#define SAVESTATE \
{ __asm \
push ACC \
push B \
push DPL \
push DPH \
push PSW \
clr RS1 \
clr RS0 \
__endasm; \
ssp[ID] = SP;\
}

#define RESTORESTATE \
{ SP = ssp[ID];\
__asm \
pop PSW \
pop DPH \
pop DPL \
pop B \
pop ACC \
__endasm; \
}

extern void main(void);

void thread_manager(void){
    do {
        ID++;
        ID = ID % MAXTHREADS;
        if(bitmap[ID]==1){
            break;
        }
    } while (1);
    RESTORESTATE;
    __asm
    reti
    __endasm;
}

void myTimer0Handler(void){
    SAVESTATE;
    tmp1 = ID;
    ID = manager_ID;
    RESTORESTATE;
    ID = tmp1;
}

void Bootstrap(void) {
    for (i=0; i<MAXTHREADS; i++) {
        bitmap[i]=0;
    }
    TMOD = 0;
    IE = 0x82;
    TR0 = 1;
    manager_ID = ThreadCreate(thread_manager);
    ID = ThreadCreate(main);
    RESTORESTATE;
}

ThreadID ThreadCreate(FunctionPtr fp) {
    EA = 0;
    for(i=0;i<MAXTHREADS;i++){
        if(bitmap[i]==0){
            break;
        }
    }
    if(i==MAXTHREADS){
        return -1;
    }

    bitmap[i] = 1;
    tmp1 = SP;
    SP = 0x3F + i*0x10;
    tmp2 = i<<3;
    __asm
    push DPL
    push DPH
    mov a,#0x00
    push a
    push a
    push a
    push a
    push _tmp2
    __endasm;
    ssp[i] = SP;
    SP = tmp1;
    EA = 1;
    return i;
}

void ThreadYield(void) {
    EA = 0;
    SAVESTATE;
    do {
        ID++;
        ID = ID % MAXTHREADS;
        if(bitmap[ID]==1){
            break;
        }
    } while (1);
    RESTORESTATE;
    EA = 1;
}

void ThreadExit(void) {
    EA = 0;
    bitmap[ID] = 0;
    do {
        ID++;
        ID = ID % MAXTHREADS;
        if(bitmap[ID]!=0){
            break;
        }
    } while (1);
    RESTORESTATE;
    EA = 1;
}
