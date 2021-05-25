#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>

typedef enum 
{
    LOGLEVEL_FATAL = 4,
    LOGLEVEL_ERROR = 3,
    LOGLEVEL_WARN = 2,
    LOGLEVEL_INFO = 1,
    LOGLEVEL_DEBUG = 0,
} LOGLEVEL;

typedef struct
{
    LOGLEVEL log_level;
    FILE *log_file;
} configuration;

configuration globalConfig;

#define true 1
#define false 0

#endif