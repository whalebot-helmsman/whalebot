#ifndef _PREFIX_H_
#define _PREFIX_H_

#include <string>

namespace prefix{
    std::string const http            =   "http://";
    std::string const https            =   "https://";
    std::string const mailto          =   "mailto:";
    std::string const javascript      =   "javascript:";

    bool    isMail(std::string const &link);
    bool    isHttp(std::string const &link);
    bool    isJavaScript(std::string const &link);
    bool    isHttps(std::string const &link);
};

#endif //_PREFIX_H_
