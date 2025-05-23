#include "CPUUsage.h"
#include "CPUUsage_Private.h"

#include <stdlib.h>
#include <inttypes.h>
#include <sys/resource.h>
#include <assert.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

static const int NANOSEC_PER_SEC = 1000000000;
static const int MICROSEC_PER_NANOSEC =  1000;

CPUUsageContext_t *CPUUsageCreate(size_t windowSize, CPUUsageForWho_t who) 
{
    CPUUsageContext_t *ctxt = malloc(sizeof(CPUUsageContext_t) + windowSize * sizeof(CPUUsageIntervalStats_t));
    assert(ctxt);
    
    _CPUUsageInitIntervalBuffer(ctxt, ctxt+1, windowSize);

    switch (who)
    {
    case CPUUsageForProcess:
        ctxt->rusageWho = RUSAGE_SELF;
        break;
/*#ifndef __MACH__
    case CPUUsageForThread:
        ctxt->rusageWho = RUSAGE_THREAD;
        break;
#endif*/
    default:
        CPUUsageFree(ctxt);
        return 0;
    }

    struct timespec curTimeSpec;
    struct rusage usage;
    int ret;

    ret = getrusage(ctxt->rusageWho, &usage);
    assert(!ret);

    ret = _CPUUsageGetTime(&curTimeSpec);
    assert(!ret);

    ctxt->lastTime = curTimeSpec;
    ctxt->lastUTime = usage.ru_utime;
    ctxt->lastSTime = usage.ru_stime;

    return ctxt; 
}

void CPUUsageFree(CPUUsageContext_t* ctxt)
{
    free(ctxt);
}

void CPUUsageUpdate(CPUUsageContext_t *ctxt)
{
    struct timespec curTimespec; 
    struct rusage usage;
    int ret;

    ret = getrusage(ctxt->rusageWho, &usage);
    assert(!ret);

    ret = _CPUUsageGetTime( &curTimespec);
    assert(!ret);

    CPUUsageIntervalStats_t *curInt =  _CPUUsageNextInterval(ctxt);
    _CPUUsageCalculateInterval(curInt, 
        &(ctxt->lastTime),  
        &curTimespec,
        &(ctxt->lastUTime),
        &(usage.ru_utime), 
        &(ctxt->lastSTime), 
        &(usage.ru_stime));

    ctxt->lastUTime = usage.ru_utime;
    ctxt->lastSTime = usage.ru_stime;
    ctxt->lastTime = curTimespec;
}


void _CPUUsagePrintInterval(FILE *outfile, CPUUsageIntervalStats_t *interval)
{
    fprintf(outfile, "Interval Start: %lld s %lld ns\n", (long long)interval->intervalStart.tv_sec, (long long)interval->intervalStart.tv_nsec);
    fprintf(outfile, "Interval Duration (ns): %ld\n", interval->durationNanoSec);
    fprintf(outfile, "User Percent: %f\n", interval->userPct);
    fprintf(outfile, "Sys Percent: %f\n", interval->sysPct);
    fprintf(outfile, "Sys Percent: %f\n", interval->overallPct);
}


void _CPUUsageCalculateInterval(CPUUsageIntervalStats_t *interval, 
    struct timespec *intervalStart, 
    struct timespec *intervalEnd,
    struct timeval *uTimeStart, 
    struct timeval *uTimeEnd, 
    struct timeval *sTimeStart, 
    struct timeval *sTimeEnd)
{
    struct timespec intervalDuration; 
    struct timeval uTimeDelta;
    struct timeval sTimeDelta;

    timespecsub(intervalEnd, intervalStart, &intervalDuration);

    timersub(uTimeEnd, uTimeStart, &uTimeDelta);

    timersub(sTimeEnd, sTimeStart, &sTimeDelta);

    uint64_t intervalDurNanoSec =  _CPUUsageTimespecToNanoSec(&intervalDuration);
    uint64_t uTimeDeltaNanoSec = _CPUUsageTimevalToNanoSec(&uTimeDelta); 
    uint64_t sTimeDeltaNanoSec = _CPUUsageTimevalToNanoSec(&sTimeDelta);

    interval->userPct = uTimeDeltaNanoSec / ((float)intervalDurNanoSec) * 100.0;
    interval->sysPct = sTimeDeltaNanoSec / ((float)intervalDurNanoSec) * 100.0;
    interval->overallPct = (uTimeDeltaNanoSec + sTimeDeltaNanoSec) / ((float)intervalDurNanoSec) * 100.0;
   
    interval->intervalStart = *intervalStart;
    interval->durationNanoSec = intervalDurNanoSec;
}


void _CPUUsageInitIntervalBuffer(CPUUsageContext_t *ctxt, void *bufferStart, size_t windowSize)
{
    ctxt->intervals = bufferStart;
    ctxt->intervalCount = 0;
    ctxt->windowStart = 0;
    ctxt->windowSize = windowSize;
}



size_t CPUUsageGetNumIntervals(CPUUsageContext_t* ctxt)
{
    return ctxt->intervalCount;
}

CPUUsageIntervalStats_t* CPUUsageGetInterval(CPUUsageContext_t *ctxt, size_t intervalIndex)
{
    assert(intervalIndex < ctxt->intervalCount);

    size_t index = (ctxt->windowStart + intervalIndex) % ctxt->windowSize;

    return &(ctxt->intervals[index]);
}

CPUUsageIntervalStats_t *_CPUUsageNextInterval(CPUUsageContext_t *ctxt)
{
    if (ctxt->intervalCount == ctxt->windowSize)
    {
        // We are full, slide the window over
        ctxt->windowStart = (ctxt->windowStart + 1) % ctxt->windowSize;
    }
    else
    {
        // Not full yet, Leave the start value alone and increment the count
        ctxt->intervalCount++;
    }
    size_t next = (ctxt->windowStart + ctxt->intervalCount - 1) % ctxt->windowSize;
    return &(ctxt->intervals[next]);
}


int _CPUUsageGetTime(struct timespec *ts)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
    return 0;
#else
    return clock_gettime(CLOCK_REALTIME, ts);
#endif
}


uint64_t _CPUUsageTimevalToNanoSec(struct timeval *tv)
{
    return (tv->tv_sec * NANOSEC_PER_SEC + tv->tv_usec * MICROSEC_PER_NANOSEC);
}


uint64_t _CPUUsageTimespecToNanoSec(struct timespec *ts)
{
    return (ts->tv_sec * NANOSEC_PER_SEC + ts->tv_nsec);
}

