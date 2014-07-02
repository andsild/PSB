#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <mutex>


namespace logging
{

enum severity_type
{
    no_output = -1,
    info,
    extensive,
    debug,
    warning,
    error
};


class log_policy_interface
{
    public:
        virtual void		open_ostream(const std::string& name) = 0;
        virtual void		close_ostream() = 0;
        virtual void		write(const std::string& msg) = 0;

};


class file_log_policy : public log_policy_interface
{
    std::unique_ptr< std::ofstream > out_stream;
    public:
    file_log_policy() : out_stream( new std::ofstream ) {}
    void open_ostream(const std::string& name);
    void close_ostream();
    void write(const std::string& msg);
    ~file_log_policy();
};


template< typename log_policy >
class logger
{
    unsigned log_line_number;
    std::string get_time();
    std::string get_logline_header();
    std::stringstream log_stream;
    log_policy* policy;
    std::mutex write_mutex;

    //Core printing functionality
    void print_impl();
    template<typename First, typename...Rest>
        void print_impl(First parm1, Rest...parm);
    public:
    logger( const std::string& name );
    int iLevel;
    bool bHeader;
    int getLevel();
    void setLevel(int);
    void setHeader(bool);

    template< severity_type severity , typename...Args >
        void print( Args...args );

    ~logger();
};


} /* EndOfNamespace */

#endif
