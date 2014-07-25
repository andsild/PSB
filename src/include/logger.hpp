#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

namespace logging
{

enum severity_type
{
    no_output = -1,
    error,
    warning,
    info,
    extensive,
    debug
};


class LogPolicyInterface
{
    public:
        virtual void        open_ostream(const std::string& name) = 0;
        virtual void        flush() = 0;
        virtual void        close_ostream() = 0;
        virtual void        write(const std::string& msg) = 0;
};

class FileLogPolicy : public LogPolicyInterface
{
    std::unique_ptr< std::ofstream > out_stream;
    public:
    FileLogPolicy() : out_stream( new std::ofstream ) {}
    void open_ostream(const std::string& name);
    void close_ostream();
    void flush();
    void write(const std::string& msg);
    ~FileLogPolicy();
};

template< typename log_policy >
class Logger
{
    private:
        unsigned log_line_number;
        std::string get_time()
        {
            std::string time_str;
            time_t raw_time;

            time( & raw_time );
            time_str = ctime( &raw_time );

            //without the newline character
            return time_str.substr( 0 , time_str.size() - 1 );
        }
        std::string get_logline_header()
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
        std::stringstream log_stream;
        log_policy* policy;
        std::mutex write_mutex;
        int iLevel;
        bool bHeader;

        //Core printing functionality
        void print_impl()
        {
            policy->write( get_logline_header() + log_stream.str() );
            log_stream.str("");
        }
        template<typename First, typename...Rest>
            void print_impl(First parm1, Rest...parm)
            {
                log_stream<<parm1;
                print_impl(parm...);
            }
        void initialize()
        {
            log_line_number = 0;
            this->iLevel = severity_type::info;
            this->bHeader = true;
            policy = new log_policy;
            if( !policy )
            {
                throw std::runtime_error("Logger: Unable to create the Logger instance"); 
            }
        }
    public:
        Logger()
        {
            this->initialize();
        }
        Logger( const std::string& name )
        {
            this->initialize();
            this->setName(name);
        }

        void setName(const std::string& name)
        {
            policy->open_ostream( name );
        }
        int getLevel()
        {
            return this->iLevel;
        }
        void setLevel(int iLevel)
        {
            this->iLevel = iLevel;
        }
        void setHeader(bool bFlag)
        {
            this->bHeader = bFlag;
        }

        template< severity_type severity , typename ...Args >
            void print( Args...args )
            {
                if(severity > this->iLevel)
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

        void flush()
        {
            policy->flush();
        }


        ~Logger()
        {
            if( policy ) {
                policy->close_ostream();
                delete policy;
            }
        }
};

} /* EndOfNamespace */

#endif
