#include <jansson.h>
#include "CPUUsage.h"
#include <assert.h>
static const char* WINDOW_ALL_INTERVALS_KEY = "allIntervals";

static const char* INTERVAL_START_TIME_KEY = "intervalStartTimespec";
static const char* INTERVAL_DURATION_KEY = "intervalDuraitonNSec";
static const char* INTERVAL_UTIME_PERCENT_KEY = "intervalUTimePercent";
static const char* INTERVAL_STIME_PERCENT_KEY = "intervalSTimePercent";
static const char* INTERVAL_OVERALL_PERCENT_KEY = "intervalOverallCPUPercent";

static const char* TIMESPEC_SEC_KEY = "tvSec";
static const char* TIMESPEC_NSEC_KEY = "tvNSec";

// "Private" functions
json_t* _CPUUsageTimespecToJSON(struct timespec *timespec);

json_t* CPUUsageIntervalToJSON(CPUUsageIntervalStats_t *interval)
{
    assert(interval);
    json_t *jInterval = json_object();
    json_t *jTimespec = _CPUUsageTimespecToJSON(&(interval->intervalStart));
    json_t *jDuration = json_integer(interval->durationNanoSec);
    json_t *jUTimePct = json_real(interval->userPct);
    json_t *jSTimePct = json_real(interval->sysPct);
    json_t *jOverallPct = json_real(interval->overallPct);

    json_object_set(jInterval, INTERVAL_START_TIME_KEY, jTimespec);
    json_object_set(jInterval, INTERVAL_DURATION_KEY, jDuration);
    json_object_set(jInterval, INTERVAL_UTIME_PERCENT_KEY, jUTimePct);
    json_object_set(jInterval, INTERVAL_STIME_PERCENT_KEY, jSTimePct);
    json_object_set(jInterval, INTERVAL_OVERALL_PERCENT_KEY, jOverallPct);

    json_decref(jTimespec);
    json_decref(jDuration);
    json_decref(jUTimePct);
    json_decref(jSTimePct);
    json_decref(jOverallPct);

    return jInterval;
}

json_t* _CPUUsageTimespecToJSON(struct timespec *timespec)
{
    assert(timespec);

    json_t *jTimespec = json_object();
    json_t *jSeconds = json_integer(timespec->tv_sec);
    json_t *jNanoSeconds = json_integer(timespec->tv_nsec);

    json_object_set(jTimespec, TIMESPEC_SEC_KEY, jSeconds);
    json_object_set(jTimespec, TIMESPEC_NSEC_KEY, jNanoSeconds);

    json_decref(jSeconds);
    json_decref(jNanoSeconds);
    
    return jTimespec;
}

void CPUUsagePrintfIntervalJSON(CPUUsageContext_t *ctxt, FILE *outfile, size_t intervalIndex)
{
    assert(ctxt);
    assert(outfile);

    CPUUsageIntervalStats_t *interval = CPUUsageGetInterval(ctxt, intervalIndex);
    if (interval)
    {

        json_t *intervalJSON = CPUUsageIntervalToJSON(interval);
        json_dumpf(intervalJSON, outfile, 0);
    }
    else 
    {
        fprintf(outfile, "Invalid interval number %ld.\n", intervalIndex);
    }
}

void CPUUsagePrintLatestIntervalJSON(CPUUsageContext_t *ctxt, FILE *outfile)
{
    assert(ctxt);

    ssize_t index = CPUUsageGetNumIntervals(ctxt) - 1;
    CPUUsagePrintfIntervalJSON(ctxt, outfile, index);
}

void CPUUsagePrintfAllIntervalsJSON(CPUUsageContext_t *ctxt, FILE *outfile)
{
    json_t *jWindow = json_object();
    json_t *jIntervalsArray = json_array();

    int numInterval = CPUUsageGetNumIntervals(ctxt);

    for (int i = 0 ; i < numInterval; i++)
    {
        CPUUsageIntervalStats_t *curInterval = CPUUsageGetInterval(ctxt, i);
        json_t *jCurInterval = CPUUsageIntervalToJSON(curInterval);
        json_array_append_new(jIntervalsArray, jCurInterval);
    }

    json_object_set(jWindow, WINDOW_ALL_INTERVALS_KEY, jIntervalsArray);
    json_decref(jIntervalsArray);
    json_dumpf(jWindow, outfile, 0);
    json_decref(jWindow);
}

