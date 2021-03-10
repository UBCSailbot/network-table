#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/named_scope.hpp>

void log_init() {
    boost::log::add_common_attributes();

    auto timeFmt = 
      boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M");

    auto fmtSeverity = boost::log::expressions::
            attr<boost::log::trivial::severity_level>("Severity");

    boost::log::formatter logFmt =
        boost::log::expressions::format("[%1%] (%2%) %3%")
        % timeFmt % fmtSeverity
        % boost::log::expressions::smessage;

    auto fsSink = boost::log::add_file_log(
            boost::log::keywords::file_name = "../logs/bbb_CANBUS_LISTENER.log",
            boost::log::keywords::scan_method = boost::log::sinks::file::scan_matching);

    fsSink->set_formatter(logFmt);
                
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
}

int main() {
  log_init();
  //init();

  BOOST_LOG_TRIVIAL(info) << "Info Log";
  BOOST_LOG_TRIVIAL(info) << "app exiting";
  BOOST_LOG_TRIVIAL(fatal) << "app exiting";
  BOOST_LOG_TRIVIAL(debug) << "12345";
  BOOST_LOG_TRIVIAL(trace) << "asdf";
  BOOST_LOG_TRIVIAL(trace) << "thinking";
  BOOST_LOG_TRIVIAL(fatal) << "IT WORKS??";

  return 0;
}
