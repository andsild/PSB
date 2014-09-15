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

/** Initialize a logger.

  @param name is the output to log to
*/
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

/** Force-flush the output-stream
*/
void FileLogPolicy::flush()
{
   if(out_stream)
        out_stream->flush();
}

/** Close a logger gracefully
*/
void FileLogPolicy::close_ostream()
{
    if( out_stream )
    {
        out_stream->close();
    }
}

/** Write something to file
  @param msg is the content to write
*/
void FileLogPolicy::write(const std::string& msg)
{
    (*out_stream)<<msg<<std::endl;
}

/** Destructor. Make sure out_stream is closed.
*/
FileLogPolicy::~FileLogPolicy()
{
    if( out_stream )
    {
        close_ostream();
    }
}

} /* EndOfNamespace */

#ifdef LOGGING /* Constructors create files in startup, so do not start logger
                  unless we are going to use them
               */
logging::Logger< logging::FileLogPolicy > log_inst(LOG_DIR "execution.log");
logging::Logger< logging::FileLogPolicy > log_inst_std("/dev/fd/1");
#endif
