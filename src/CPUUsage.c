#include "CPUUsage.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>

static const int NANOSEC_PER_SEC = 1000000000;
static const int MICROSEC_PER_SEC =   1000000;

CPUUsageContext_t *CPUUsageCreate(unsigned int windowSize, CPUUsageForWho_t who) 
{
    CPUUsageContext_t *ctxt = malloc(sizeof(CPUUsageContext_t) + windowSize * sizeof(CPUUsageIntervalStats_t));
    assert(ctxt);
    
    _CPUUsageInitIntervalBuffer(ctxt, context+1, windowSize);

    switch (who)
    {
        case CPUUsageForProcess:
            ctxt->who = RUSAGE_SELF;
            break;
        case CPUUsageForThread:
            ctxt->who = RUSAGE_THREAD;
            break;
        default:
            CPUUsageFree(ctxt);
            return 0;
    }

    struct timespec curTimeSpec;
    struct rusage usage;
    int ret;

    ret = getrusage(ctxt->rusageWho, &usage);
    assert(!ret);

    ret = clock_gettime(CLOCK_MONOTONIC, &curTimeSpec);
    assert(!ret);

    ctxt->lastTime = curTimeSpec;
    ctxt->lastUTime = usage.ru_utime;
    ctxt->lastSTime = usage.ru_stime;

    return ctxt; 
}

void CPUusageFree(CPUUsageContext_t* ctxt)
{
    free(ctxt);
}

void CPUUsageUpdate()
{
    struct timespec curTimespec; 
    struct rusage usage;
    int ret;

    ret = getrusage(ctxt->rusageWho, &usage);
    assert(!ret);

    ret = clock_gettime(CLOCK_MONOTONIC, &curTimeSpec);
    assert(!ret);

    CPUUsageIntervalStats_t *curInt =  _CPUUsageNextInterval(ctxt);
    _CPUUsageCaclulateInterval(curInt, 
        &(ctxt->lastTime),  
        &curTimeSpec,
        &(ctxt->lastUTime),
        &(usage.ru_utime), 
        &(ctxt->lastSTime), 
        &(usage.ru_stime));

    ctxt->lastUtime = usage.ru_utime;
    ctxt->lastSTime = usage.ru_stime;
    ctxt->lastTime = curTimeSpec;
}


void CPUUsagePrintLatestInterval()
{
}


void CPUUsagePrintAllIntervals()
{
}


void _CPUUsageCalculateInterval(CPUUsageIntervalStats_t *interval, 
    struct timepsec *intervalStart, 
    struct timespec *intervalEnd,
    struct timeval *uTimeStart, 
    struct timeval *uTimeEnd, 
    struct timeval *sTimeStart, 
    struct timeval *sTimeEnd)
{
    struct timespec intervalDuration; 
    struct timeval uTimeDelta;
    struct timeval sTimeDelta;

    timersub(intervalEnd, intervalStart, &intervalDuration);

    timersub(uTimeEnd, uTimerStart, &uTimeDelta);

    timesub(sTimeEnd, sTimeStart, &sTimeDelta);

    uint64_t intervalDurNanoSec =  _CPUUsageTimespecToNanoSec(&intervalDuration);
    uint64_t uTimeDeltaNanoSec = _CPUUsageTimevalToNanoSec(&uTimeDelta); 
    uint64_t sTimeDeltaNanoSec = _CPUusageTimevalToNanoSec(&sTimeDelta);

    interval->userPct = uTimeDeltaNanoSec / ((float)intervalDurNanoSec);
    interval->sysPct = sTimeDeltaNanoSec / ((float)intervalDurNanoSec);
    interval->overallPct = (uTimeDeltaNanoSec + sTimeDeltaNanoSec) / ((float)intervalDurNanoSec);
   
    interval->intervalStart = *intervalStart;
    interval->durationNanoSec = intervalDurNanoSec;
}


void _CPUUsageInitIntervalBuffer(CPUUsageContext_t *ctxt, void *bufferStart, unsigned int intervalCount);
{
    ctxt->intervals = bufferStart;
    ctxt->windowStart = ctxt->intervals;
    ctxt->windowEnd ctxt->intervals;
    ctxt->intervalCount = intervalCount;
}


uint64_t _CPUUsageTimevalToNanoSec(struct timeval *tv)
{
    return (tv->tv_sec * NANOSEC_PER_SEC + tv->tv_usec * MICROSEC_PER_SEC);
}


uint64_t _CPUUsageTimespecToNanoSec(struct timespec *ts)
{
    return (ts->tv_sec * NANOSEC_PER_SEC + ts->tv_nsec);
}

