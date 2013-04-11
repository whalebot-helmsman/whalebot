#include <iostream>

#include <boost/algorithm/string.hpp>

#include <parserbase.h>

#include <whale_link.h>

CLink::CLink()
: m_tGurl()
, m_sCookie("")
, m_sCookieForCut("")
{}

CLink::CLink(const CLink& father, const std::string& relUrl )
: m_tGurl(father.m_tGurl.Resolve(relUrl))
, m_sCookie(father.m_sCookie)
, m_sCookieForCut(father.m_sCookieForCut)
{}

CLink::CLink( const std::string& url )
: m_tGurl(url)
, m_sCookie("")
, m_sCookieForCut("")
{}

std::string CLink::getServer()const{
    return m_tGurl.host();
}
std::string CLink::getUri()const{
    return m_tGurl.PathForRequest();
}
std::string  const &CLink::getCookie()const{
    return m_sCookie;
}

std::string const &CLink::getCookieForCut()const{
    return m_sCookieForCut;
}

void CLink::setCookie(std::string const &cookie){
    m_sCookie    =   cookie;
    if(!m_sCookie.empty()){
        CParserBase   parser(";");
        parser.parseString(m_sCookie);
        m_sCookieForCut  =   parser.unsafe_getElement(0);
        boost::to_lower(m_sCookieForCut);
    }
}

bool CLink::isValid()const{
    return m_tGurl.is_valid();
}
void CLink::nil(){
    m_tGurl =  GURL::EmptyGURL();
    m_sCookie.clear();
    m_sCookieForCut.clear();
}

std::string CLink::toString()const {
    std::string ret("http://");
    ret +=  getServer();
    ret +=  getUri();
    return ret;
}

void CLink::setGurl(const GURL& gurl)
{
    m_tGurl =   gurl;
}


std::ostream& operator<<(std::ostream& s,  CLink const & t){
    s << "http://" << t.getServer() <<std::endl;
    s << t.getUri() << std::endl;
    s << t.getCookie() << std::endl;

    return s;
}


std::istream& operator>>(std::istream& s,  CLink& t){
    t.nil();

    std::string host("");
    std::string req("");

    if(not std::getline(s, host)){
        return s;
    }

    if(not std::getline(s, req)){
        return s;
    }



    t.setGurl(GURL(host + req));

    std::string cookie("");

    if(std::getline(s, cookie)){
        t.setCookie(cookie);
    }

    return s;

}

bool CLink::operator==(CLink const& another)const{
    return (  (m_tGurl.PathForRequest() == another.m_tGurl.PathForRequest())
           && (m_tGurl.host() == another.m_tGurl.host()) );
}
