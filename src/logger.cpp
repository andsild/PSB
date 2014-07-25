#include "logger.hpp"

#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

#include "file.hpp"
#include "loginstance.hpp"

namespace logging
{

void FileLogPolicy::open_ostream(const std::string& name)
{
    std::string sLog = LOG_DIR;
    file_IO::mkdirp(sLog.c_str());
    out_stream->open( name.c_str(), std::ios_base::binary|std::ios_base::out );
    if( !out_stream->is_open() ) 
    {
        std::string sMsg = "Logger: unable to open an output stream: " + name;
        throw(std::runtime_error(sMsg.c_str()));
    }
}

void FileLogPolicy::flush()
{
    if(out_stream)
        out_stream->flush();
}

void FileLogPolicy::close_ostream()
{
    if( out_stream )
    {
        out_stream->close();
    }
}

void FileLogPolicy::write(const std::string& msg)
{
    (*out_stream)<<msg<<std::endl;
}

FileLogPolicy::~FileLogPolicy()
{
    if( out_stream )
    {
        close_ostream();
    }
}

} /* EndOfNamespace */

logging::Logger< logging::FileLogPolicy > log_inst(LOG_DIR "execution.log");
logging::Logger< logging::FileLogPolicy > log_inst_std("/dev/fd/0");
