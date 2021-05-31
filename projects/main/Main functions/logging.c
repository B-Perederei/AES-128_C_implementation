#include "logging.h"

void print_time(FILE *f)
{
    struct tm *tm_info;
    time_t now;
    time(&now);
    tm_info = gmtime(&now);
    char buffer[26];
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S ", tm_info);
    fputs(buffer, f);
}