#include "logger.hpp"

#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

namespace logging
{

void FileLogPolicy::open_ostream(const std::string& name)
{
    out_stream->open( name.c_str(), std::ios_base::binary|std::ios_base::out );
    // file_IO::mkdirp(name.c_str());
    if( !out_stream->is_open() ) 
    {
        std::string sMsg = "Logger: unable to open an output stream: " + name;
        throw(std::runtime_error(sMsg.c_str()));
    }
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
