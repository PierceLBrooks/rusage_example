#include "CPUUsage.h" 
#include <stdint.h>
CPUUsageIntervalStats_t *_CPUUsageNextInterval(CPUUsageContext_t *ctxt);

void _CPUUsageInitIntervalBuffer(CPUUsageContext_t* ctxt, void* bufferStart, size_t windowSize);

int _CPUUsageGetTime(struct timespec *ts);

void _CPUUsageCalculateInterval(CPUUsageIntervalStats_t *interval, 
    struct timespec *intervalStart, 
    struct timespec *intervalEnd,
    struct timeval *uTimeStart, 
    struct timeval *uTimeEnd, 
    struct timeval *sTimeStart, 
    struct timeval *sTimeEnd);
uint64_t _CPUUsageTimevalToNanoSec(struct timeval *tv);
uint64_t _CPUUsageTimespecToNanoSec(struct timespec *ts);

# define timespecsub(a, b, result)                          \
do {                                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;           \
    (result)->tv_nsec = (a)->tv_nsec - (b)->tv_nsec;        \
    if ((result)->tv_nsec < 0)                              \
    {                                                       \
        --(result)->tv_sec;                                 \
          (result)->tv_nsec += 1000000000;                  \
    }                                                       \
} while (0)
