#ifndef LOGINSTANCE_H
#define  LOGINSTANCE_H

#include "logger.hpp"

#define LOG_DIR "./log/"

extern logging::Logger< logging::FileLogPolicy > log_inst;
extern logging::Logger< logging::FileLogPolicy > log_inst_std;

#ifndef LOGGING
    #define LOGGING 0
#else
    #define LOGGING 1
#endif

#define MLOG(x, ...) if(LOGGING) {CLOG(x)(__VA_ARGS__);LOG(x)(__VA_ARGS__); }

#define DO_IF_LOGLEVEL(x) if(LOGGING && (x <= log_inst.getLevel() \
                                      || x <= log_inst_std.getLevel()))

#define MFLUSH log_inst.flush(); log_inst_std.flush()

#define LOG(x) log_inst.print< x >
#define CLOG(x) (log_inst_std.print< x >)
#define SETLEVEL(x) log_inst.setLevel(x)
#define CSETLEVEL(x) log_inst_std.setLevel(x)
#define GETLEVEL log_inst.getLevel()
#define CGETLEVEL log_inst_std.getLevel()

#endif
