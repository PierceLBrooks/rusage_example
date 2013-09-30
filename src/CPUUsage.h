#ifndef __CPUUSAGE_H_
#define __CPUUSAGE_H_

#include <stdio.h>
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


void CPUUsagePrintAllIntervals(CPUUsageContext_t *ctxt, FILE *outfile);

#ifdef CPUUSAGE_HAVE_JANSSON
#include <jansson.h>
void CPUUsagePrintfAllIntervalsJSON(CPUUsageContext_t *ctxt, FILE *outfile);

void CPUUsagePrintfIntervalJSON(CPUUsageContext_t *ctxt, FILE *outfile, size_t intervalIndex);

void CPUUsagePrintLatestIntervalJSON(CPUUsageContext_t *ctxt, FILE *outfile);

json_t* CPUUsageIntervalToJSON(CPUUsageIntervalStats_t *interval);

json_t* CPUUsageWindowToJSON(CPUUsageContext_t *ctxt);
#endif //CPUUSAGE_HAVE_JANSON

#endif /* __CPUUSAGE_H_ */

