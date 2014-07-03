#include "logger.hpp"

#include <ios>
#include <stdexcept>

// #include "./file.hpp" // for mkdirp


/*
 * Implementation which allow to write into a file
 */

namespace logging
{

void file_log_policy::open_ostream(const std::string& name)
{
    out_stream->open( name.c_str(), std::ios_base::binary|std::ios_base::out );
    // file_IO::mkdirp(name.c_str());
    if( !out_stream->is_open() ) 
    {
        std::string sMsg = "Logger: unable to open an output stream: " + name;
        throw(std::runtime_error(sMsg.c_str()));
    }
}

void file_log_policy::close_ostream()
{
    if( out_stream )
    {
        out_stream->close();
    }
}

void file_log_policy::write(const std::string& msg)
{
    (*out_stream)<<msg<<std::endl;
}

file_log_policy::~file_log_policy()
{
    if( out_stream )
    {
        close_ostream();
    }
}

template< typename log_policy >
int logger< log_policy >::getLevel(){ return this->iLevel; }

template< typename log_policy >
void logger< log_policy >::setLevel(int iLevel){ this->iLevel = iLevel; }

template< typename log_policy >
void logger< log_policy >::setHeader(bool bFlag) { this->bHeader = bFlag; }

template< typename log_policy >
    template< severity_type severity , typename...Args >
void logger< log_policy >::print( Args...args )
{
    if(this->iLevel < severity)
        return;
    write_mutex.lock();
    switch( severity )
    {
        case severity_type::debug:
            log_stream<<"<DEBUG> :";
            break;
        case severity_type::warning:
            log_stream<<"<WARNING> :";
            break;
        case severity_type::error:
            log_stream<<"<ERROR> :";
            break;
        case severity_type::extensive:
            log_stream<<"<EXTRA> :";
            break;
    };
    print_impl( args... );
    write_mutex.unlock();
}


template< typename log_policy >
void logger< log_policy >::print_impl()
{
    policy->write( get_logline_header() + log_stream.str() );
    log_stream.str("");
}

template< typename log_policy >
    template<typename First, typename...Rest >
void logger< log_policy >::print_impl(First parm1, Rest...parm)
{
    log_stream<<parm1;
    print_impl(parm...);
}
    template< typename log_policy >
std::string logger< log_policy >::get_time()
{
    std::string time_str;
    time_t raw_time;

    time( & raw_time );
    time_str = ctime( &raw_time );

    //without the newline character
    return time_str.substr( 0 , time_str.size() - 1 );
}

    template< typename log_policy >
std::string logger< log_policy >::get_logline_header()
{
    if(! this->bHeader) return std::string("");
    std::stringstream header;

    header.str("");
    header.fill('0');
    header.width(7);
    header << log_line_number++ <<" < "<<get_time()<<" - ";

    header.fill('0');
    header.width(7);
    header <<clock()<<" > ~ ";

    return header.str();
}
    template< typename log_policy >
logger< log_policy >::logger( const std::string& name )
{
    log_line_number = 0;
    this->iLevel = severity_type::info;
    policy = new log_policy;
    if( !policy )
    {
        throw std::runtime_error("LOGGER: Unable to create the logger instance"); 
    }
    policy->open_ostream( name );
}

    template< typename log_policy >
logger< log_policy >::~logger()
{
    if( policy )
    {
        policy->close_ostream();
        delete policy;
    }
}

} /* EndOfNamespace */
