#ifndef __CPUUSAGE_H_
#define __CPUUSAGE_H_

#include "CPUUsage.h"

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
    long duartionNanoSec;
} CPUUsageIntervalStats_t;


typedef struct CPUUsageContext__st {
    
    int rusageWho;
    
    struct timeval lastUTime;
    struct timeval lastSTime;
    struct timespec lastTime;

    unsigned int intervalCount;
    CPUUsageIntervalStats_t *intervals; 
    CPUUsageIntervalStats_t *windowStart;
    CPUUsageIntervalStats_t *windowEnd;
   
    void *userContext;

} CPUUsageContext_t;

CPUUsageContext_t *CPUUsageCreate(unsigned int windowSize); 
void CPUusageFree(CPUUsageContext_t* ctxt);

void CPUUsageUpdate();

void CPUUsagePrintLatestInterval();

void CPUUsagePrintAllIntervals();

#endif /* __CPUUSAGE_H_ */

