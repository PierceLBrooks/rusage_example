#include "CPUUsage.h"
#include "CPUUsage_Private.h"

#include <stdio.h>

void CPUUsagePrintInterval(CPUUsageContext_t *ctxt, FILE *outfile,  size_t intervalIndex)
{
    CPUUsageIntervalStats_t *interval = CPUUsageGetInterval(ctxt, intervalIndex);
    
    if (interval)
    {
        _CPUUsagePrintInterval(outfile, interval);
    }
    else 
    {
        fprintf(outfile, "Invalid interval number %ld.\n", intervalIndex);
    }
}

void CPUUsagePrintLatestInterval(CPUUsageContext_t *ctxt, FILE *outfile)
{
    ssize_t index = CPUUsageGetNumIntervals(ctxt) - 1;

    CPUUsagePrintInterval(ctxt, outfile, index);
}

void CPUUsagePrintAllIntervals(CPUUsageContext_t *ctxt, FILE *outfile)
{
    ssize_t numIntervals = CPUUsageGetNumIntervals(ctxt);

    fprintf(outfile, "CPUUsage Window Contents: \n");

    for (int i = 0; i < numIntervals; i++)
    {
        fprintf(outfile, "Interval %d:\n", i);
        CPUUsagePrintInterval(ctxt, outfile, i);
    }

    fprintf(outfile, "End CPUUsage Window\n");
}


