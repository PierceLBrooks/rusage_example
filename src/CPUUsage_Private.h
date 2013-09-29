#include "CPUUsage.h" 

CPUUsageIntervalStats_t *_CPUUsageNextInterval(CPUUsageContext_t *ctxt);

void _CPUUsageInitIntervalBuffer(CPUUsageContext_t* ctxt, void* bufferStart, unsigned int windowSize);

int _CPUUsageIntervalBufferIsEmpty(CPUUsageContext_t *ctxt);

void _CPUUsageCalculateInterval(CPUUsageIntervalStats_t *interval, 
    struct timepsec *intervalStart, 
    struct timespec *intervalEnd,
    struct timeval *uTimeStart, 
    struct timeval *uTimeEnd, 
    struct timeval *sTimeStart, 
    struct timeval *sTimeEnd);

uint64_t _CPUUsageTimevalToNanoSec(struct timeval *tv);
uint64_t _CPUUsageTimespecToNanoSec(struct timespec *ts);
