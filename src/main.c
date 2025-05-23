#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "CPUUsage.h"
#include "CPUUsage_Private.h"
int main()
{
    CPUUsageContext_t *ctxt = CPUUsageCreate(2, CPUUsageForProcess);

    usleep(1000000);

    CPUUsageUpdate(ctxt);

    struct timespec startts;
    struct timespec nowts;
    _CPUUsageGetTime(&startts);
    nowts = startts;
    while(nowts.tv_sec - startts.tv_sec < 10 || nowts.tv_nsec >= startts.tv_nsec)
    {
        _CPUUsageGetTime(&nowts);
    }

    CPUUsageUpdate(ctxt);

    _CPUUsageGetTime(&startts);
    nowts = startts;

    while(nowts.tv_sec - startts.tv_sec < 5 || nowts.tv_nsec >= startts.tv_nsec)
    {
        _CPUUsageGetTime(&nowts);
    }
    
    sleep(5);

    CPUUsageUpdate(ctxt);   
#ifdef CPUUSAGE_HAVE_JANSSON 
    CPUUsagePrintfAllIntervalsJSON(ctxt, stdout);
#else
    CPUUsagePrintAllIntervals(ctxt, stdout);
#endif
    printf("done");
}

