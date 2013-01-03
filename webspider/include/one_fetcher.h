/* 
 * File:   OneFetcher.h
 * Author: hotdox
 *
 * Created on February 21, 2009, 9:38 PM
 */

#ifndef _ONEFETCHER_H
#define	_ONEFETCHER_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>


const unsigned int external_error(600);

class CLink;
class CHeaderParser;

class COneFetcher{
public:
			COneFetcher();
    bool    		connect(CLink const &link);
    bool    		request(CLink const &link);
    unsigned int	getHeader(CHeaderParser &header, std::ostream &out);
    bool		getResponse(std::ostream &out);
private:
    boost::asio::io_service         m_ioService;
    boost::asio::ip::tcp::socket    m_socket;
};



#endif	/* _ONEFETCHER_H */

