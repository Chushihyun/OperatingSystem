//
//  preemptive.c
//  
//
//  Created by 朱世耘 on 2020/1/9.
//

#include <8051.h>
#include "preemptive.h"
__data __at (0x5C) char ssp[MAXTHREADS];
__data __at (0x24) char bitmap[MAXTHREADS];
__data __at (0x4A) char ID;
__data __at (0x29) char manager_ID;
__data __at (0x2A) char tmp1;
__data __at (0x2B) char tmp2;
__data __at (0x2C) char i;
__data __at (0x30) unsigned char time;
__data __at (0x2D) char count;
__data __at (0x6C) unsigned char d_time[MAXTHREADS];
__data __at (0x7F) char var;
__data __at (0x7E) char cnt;

/*
* define a macro for saving the context of the current thread by
* 1) push ACC, B register, Data pointer registers (DPL, DPH), PSW
* 2) save SP into the saved Stack Pointers array
*   as indexed by the current thread ID.
*/
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

unsigned char now(void){
    return time;
    // time++ when thread[1](main) get the power
}
void delay(unsigned char n){
    d_time[ID]=now()+n;
    // set bitmap[]=7 represent delay
    bitmap[ID]=7;
    ThreadYield();
}

void thread_manager(void){
    // find next avalible thread
    do {
        ID++;
        ID = ID % MAXTHREADS;
        if(bitmap[ID]==1){
            break;
        }
    }while (1);
    // check if time should increase
    if (ID==1) {
        cnt++;
        if (cnt==3) {
            cnt=0;
            time++;
        }
    }
    RESTORESTATE;
    __asm
    reti
    __endasm;
}

void myTimer0Handler(void){
    SAVESTATE;
    /*
    count++;
    if (count==4) {
        count=0;
        time++;
    }
    */
    // check if any delay thread times up
    for (i=0; i<MAXTHREADS; i++) {
        if ((bitmap[i]==7) && (d_time[i]==time)) {
            bitmap[i]=1;
        }
    }
    tmp1 = ID;
    ID = manager_ID;
    RESTORESTATE;
    ID = tmp1;

}

void Bootstrap(void) {
    // initialize
    for (i=0; i<MAXTHREADS; i++) {
        bitmap[i]=0;
    }
    TMOD = 0;
    IE = 0x82;
    TR0 = 1;
    time=1;
    count=0;
    cnt=0;
    SemaphoreCreate(thread_count, 4);
    manager_ID = ThreadCreate(thread_manager);
    ID = ThreadCreate(main);
    RESTORESTATE;
}

ThreadID ThreadCreate(FunctionPtr fp) {
    SemaphoreWait(thread_count);
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
    // find next avalible thread
    do {
        ID++;
        ID = ID % MAXTHREADS;
        if(bitmap[ID]==1){
            break;
        }
    } while (1);
    // check if time should increase
    if (ID==1) {
        cnt++;
        if (cnt==3) {
            cnt=0;
            time++;
        }
    }
    RESTORESTATE;
    EA = 1;
}

void ThreadExit(void) {
    // adjust Semaphore and set bitmap = 0
    SemaphoreSignal(thread_count);
    EA = 0;
    bitmap[ID] = 0;
    // find next avalible thread
    do {
        ID++;
        ID = ID % MAXTHREADS;
        var=bitmap[ID];
        if( var == 1 ){//can`t use bitmap[ID]==1
            break;
        }
    } while (1);
    // check if time should increase
    if (ID==1) {
        cnt++;
        if (cnt==3) {
            cnt=0;
            time++;
        }
    }
    RESTORESTATE;
    EA = 1;
}
