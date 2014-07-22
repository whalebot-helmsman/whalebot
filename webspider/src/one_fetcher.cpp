
#include <header_parser.h>
#include <one_fetcher.h>
#include <version.h>
#include <link/whale_link.h>
#include <options/options.hpp>


COneFetcher::COneFetcher( boost::asio::io_service& service
                        , const CFetchOptions& options )
: m_ioService(service)
, m_socket(m_ioService)
, m_options(options)
{}

bool COneFetcher::connect(CLink const &link){
    //add HTTP_0.9

    bool	ret(false);
    try{
        boost::asio::ip::tcp::resolver              resolver(m_ioService);
        boost::asio::ip::tcp::resolver::query       query(link.getServer(), "http");
        boost::asio::ip::tcp::resolver::iterator    endpoint_iterator = resolver.resolve(query);
        boost::asio::ip::tcp::resolver::iterator    end;
        boost::system::error_code                   error   =   boost::asio::error::host_not_found;

        while (error && endpoint_iterator != end){
            m_socket.close();
            m_socket.connect(*endpoint_iterator++, error);
        }

        if(!error){
            ret =   true;
        }

    }catch(...){
        ret =   false;
    }
    return ret;
}
bool COneFetcher::request(CLink const &link){
    bool	ret(true);
    try{

        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET " <<link.getUri() << " HTTP/1.0\r\n";
        request_stream << "Host: " << link.getServer() << "\r\n";
        request_stream << "Accept: */*\r\n";

        const std::string& referer(link.getReferer());
        if ((true == m_options.IsUseReferer) && (false == referer.empty())) {
            request_stream << "Referer: " << referer << "\r\n";
        }

        request_stream << "User-Agent: whalebot " << kVersion << "(http://code.google.com/p/whalebot/wiki/ForAdministrators) \r\n";
        std::string const &cookie(link.getCookieForCut());
        if(!cookie.empty())
            request_stream << "Cookie: "<<cookie<<"\r\n";
        request_stream << "Connection: close\r\n\r\n";

        boost::asio::write(m_socket, request);
    }catch(...){
        ret =   false;
    }
    return ret;
}
unsigned int COneFetcher::getHeader(CHeaderParser &header, std::ostream &out){
    unsigned int	status_code;
    try{

        boost::asio::streambuf	response;
        boost::asio::read_until(m_socket, response, "\r\n");

        std::istream	response_stream(&response);
        std::string     http_version,
                        status_message;

        response_stream >> http_version;
        response_stream >> status_code;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/"){

          return external_error;
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(m_socket, response, "\r\n\r\n");
        header.clear();

        // Process the response headers.
        std::string header1;
        while (std::getline(response_stream, header1) && header1 != "\r"){
            header.addString(header1);
        }

        // Write whatever content we already have to output.
        if (response.size() > 0){
            out << &response ;
         }

    }catch(...){
        status_code =   external_error + 1;
    }
    return status_code;
  }

bool COneFetcher::getResponse(std::ostream &out) {
    boost::asio::streambuf response;

    boost::system::error_code error;
    // Read until EOF, writing data to output as we go.
    while (boost::asio::read(m_socket, response, boost::asio::transfer_at_least(1), error)) {
        out << &response;
    }
    return error == boost::asio::error::eof;
}
