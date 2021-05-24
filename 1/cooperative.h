
#ifndef __COOPERATIVE_H__
#define __COOPERATIVE_H__

#define MAXTHREADS 4

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);

#endif // __COOPERATIVE_H__
