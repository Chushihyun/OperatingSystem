//
//  preemptive.h
//  
//
//  Created by 朱世耘 on 2020/1/9.
//
#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4

#define CNAME(s) _ ## s
#define PLUS1(s) s ## $
#define PLUS(s) PLUS1(s)
#define SemaphoreCreate(s, n) s=n;
//  signal() semaphore s
#define SemaphoreSignal(s) \
{ \
__asm \
inc CNAME(s) \
__endasm; \
}
// do busy-wait() on semaphore s
#define SemaphoreWait(s)\
{\
    SemaphoreWaitBody(s,PLUS(__COUNTER__)) \
}

#define SemaphoreWaitBody(s, label) \
    { __asm \
label: \
    mov ACC , CNAME(s)\
    jz label \
    jb ACC.7 , label \
    dec CNAME(s) \
    __endasm;\
}


typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);

#endif
