#ifndef LOGINSTANCE_H
#define  LOGINSTANCE_H

#include "logger.hpp"

static logging::Logger< logging::FileLogPolicy > log_inst("exe.log");
static logging::Logger< logging::FileLogPolicy > log_inst_std("/dev/fd/0");

#define LOG(x) log_inst.print< x >
#define CLOG(x) (log_inst_std.print< x >)
#define DO_IF_LOGLEVEL(x) if(x >= log_inst.getLevel()) 
#define SETLEVEL(x) log_inst.setLevel(x)
#define CSETLEVEL(x) log_inst_std.setLevel(x)

#endif
