#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <statsem_string/include/parserbase.h>

#include "whale_link.h"

const char   CLink::KHostFieldName[]    =   "host";
const char   CLink::KPathFieldName[]    =   "path";
const char   CLink::KCookieFieldName[]  =   "cookie";
const char   CLink::KRefererFieldName[] =   "referer";

CLink::CLink()
: m_tGurl()
, m_sCookie("")
, m_sCookieForCut("")
, m_sReferer("")
{}

CLink::CLink(const CLink& father, const std::string& relUrl )
: m_tGurl(father.m_tGurl.Resolve(relUrl))
, m_sCookie(father.m_sCookie)
, m_sCookieForCut(father.m_sCookieForCut)
, m_sReferer(father.toString())
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

void CLink::setReferer(const std::string& referer)
{
    m_sReferer  =   referer;
}

bool CLink::isValid()const{
    return m_tGurl.is_valid();
}
void CLink::nil(){
    m_tGurl =  GURL::EmptyGURL();
    m_sCookie.clear();
    m_sCookieForCut.clear();
    m_sReferer.clear();
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


std::ostream& operator << (std::ostream& s,  CLink const & t) {
    boost::property_tree::ptree urlInJson;

    urlInJson.put(CLink::KHostFieldName, t.getServer());
    urlInJson.put(CLink::KPathFieldName, t.getUri());

    const std::string&  cookie(t.getCookie());
    if (false == cookie.empty()) {
        urlInJson.put(CLink::KCookieFieldName, cookie);
    }

    const std::string&  referer(t.getReferer());
    if (false == referer.empty()) {
        urlInJson.put(CLink::KRefererFieldName, referer);
    }

    write_json(s, urlInJson, false);

    return s;
}


std::istream& operator>>(std::istream& s,  CLink& t) {
    t.nil();

    std::string buffer;

    if (std::getline(s, buffer)) {
        return s;
    }

    boost::property_tree::ptree urlInJson;
    std::stringstream           stream(buffer);

    read_json(stream, urlInJson);

    std::string host    =   "http://";
    host    +=  urlInJson.get<std::string>(CLink::KHostFieldName);
    std::string path    =   urlInJson.get<std::string>(CLink::KPathFieldName);
    t.setGurl(GURL(host + path));

    t.setCookie(urlInJson.get(CLink::KCookieFieldName, ""));
    t.setReferer(urlInJson.get(CLink::KRefererFieldName, ""));


    return s;
}

bool CLink::operator==(CLink const& another)const{
    return (  (m_tGurl.PathForRequest() == another.m_tGurl.PathForRequest())
           && (m_tGurl.host() == another.m_tGurl.host()) );
}

std::string const &  CLink::getReferer()const
{
    return m_sReferer;
}
