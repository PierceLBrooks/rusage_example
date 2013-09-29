#ifndef __CPUUSAGE_H_
#define __CPUUSAGE_H_

#include <time.h>
#include <sys/time.h>

typedef enum CPUUsageForWho
{
    CPUUsageForProcess = 1,
    CPUUsageForThread = 2,
} CPUUsageForWho_t;


typedef struct CPUUsageIntervalStats_st {
    // CPU Usage percentages
    double userPct;
    double sysPct; 
    double overallPct;

    // Time interval start and duration
    struct timespec intervalStart; 
    long durationNanoSec;
} CPUUsageIntervalStats_t;


typedef struct CPUUsageContext__st {
    
    int rusageWho;
    
    struct timeval lastUTime;
    struct timeval lastSTime;
    struct timespec lastTime;

    size_t windowSize; 
    size_t intervalCount;
    size_t windowStart;
    CPUUsageIntervalStats_t *intervals; 
   
    void *userContext;

} CPUUsageContext_t;

CPUUsageContext_t *CPUUsageCreate(size_t windowSize, CPUUsageForWho_t who); 

void CPUUsageFree(CPUUsageContext_t* ctxt);

void CPUUsageUpdate();

size_t CPUUsageGetNumIntervals(CPUUsageContext_t* ctxt);

CPUUsageIntervalStats_t* CPUUsageGetInterval(CPUUsageContext_t *ctxt, size_t intervalIndex);

void CPUUsagePrintLatestInterval();

void CPUUsagePrintAllIntervals();

#endif /* __CPUUSAGE_H_ */

