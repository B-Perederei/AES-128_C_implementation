#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "global.h"

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

void print_time(FILE *f);
 
#define LOG(loglevel, ...) \
    if (loglevel >= globalConfig.log_level) { \
        print_time(globalConfig.log_file); \
        switch (loglevel) { \
            case LOGLEVEL_DEBUG: fprintf(globalConfig.log_file, "DEBUG "); break; \
            case LOGLEVEL_INFO: fprintf(globalConfig.log_file, "INFO "); break; \
            case LOGLEVEL_WARN: fprintf(globalConfig.log_file, "WARN "); break; \
            case LOGLEVEL_ERROR: fprintf(globalConfig.log_file, "ERROR "); break; \
            case LOGLEVEL_FATAL: fprintf(globalConfig.log_file, "FATAL "); break; \
        }\
        fprintf(globalConfig.log_file, "%s:%d ", __FILENAME__, __LINE__); \
        fprintf(globalConfig.log_file, __VA_ARGS__); \
        fputs("\n", globalConfig.log_file); \
    } 

#endif