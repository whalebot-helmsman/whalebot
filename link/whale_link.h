/*
 * File:   link.h
 * Author: hotdox
 *
 * Created on March 21, 2009, 8:45 PM
 */

#ifndef _LINK_H
#define	_LINK_H
#include <string>
#include <boost/functional/hash.hpp>

#include <gurl.h>


class CLink{
public:

                        CLink();
                        CLink(const CLink& father, const std::string& relUrl );
                        CLink( const std::string& url );
    std::string         getServer()const;
    std::string         getUri()const;
    std::string const & getCookie()const;
    std::string const & getCookieForCut()const;
    std::string         toString()const;

    std::string         getPortOrScheme()const;

    std::string const & getReferer()const;

    void                setCookie(std::string const &cookie);
    void                setReferer(const std::string& referer);
    void                setGurl(const GURL& gurl);
    bool                isValid()const;
    void                nil();
    bool                operator==(CLink const& another)const;

    static const char   KHostFieldName[];
    static const char   KPathFieldName[];
    static const char   KCookieFieldName[];
    static const char   KRefererFieldName[];

private:

    GURL        m_tGurl;
    std::string m_sCookie,
                m_sCookieForCut;
    std::string m_sReferer;
};

std::ostream& operator<<(std::ostream& s,  CLink const & t);
std::istream& operator>>(std::istream& s,  CLink & t);

struct ihash
    : std::unary_function<CLink, std::size_t>
{
    std::size_t operator()(CLink const& x) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, x.getServer());
        boost::hash_combine(seed, x.getUri());
        return seed;

    }
};

#endif	/* _LINK_H */

