#include <stdio.h>
#include <time.h>
#include "dtc_logger.h"

typedef struct {
    const char *code;
    time_t      timestamp;
} DTC_Record;

void log_dtc(const char *dtc_code)
{
    DTC_Record record;
    record.code      = dtc_code;
    record.timestamp = time(NULL);

    FILE *fp = fopen("dtc_log.txt", "a");
    if (!fp) {
        printf("[LOGGER ERROR] Could not open dtc_log.txt for writing.\n");
        return;
    }

    char time_str[64];
    struct tm *tm_info = localtime(&record.timestamp);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(fp, "[%s] DTC Logged: %s\n", time_str, record.code);
    fclose(fp);
}
