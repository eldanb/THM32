#pragma once

#define LOGLEVEL_TRACE 5
#define LOGLEVEL_DEBUG 4
#define LOGLEVEL_INFO 3
#define LOGLEVEL_WARNING 2
#define LOGLEVEL_ERROR 1

void log(int debuglevel, const char *fmt, ...);
