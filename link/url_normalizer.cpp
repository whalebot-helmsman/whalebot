#include "url_normalizer.hpp"
#include "iacceptor.h"
#include "prefix.h"
#include "whale_link.h"

CUrlNormalizer::CUrlNormalizer(const std::string& storagePath)
: ParentLink(NULL)
, Acceptor(NULL)
, Storage(storagePath.c_str(), std::ios_base::app)
{
}

void CUrlNormalizer::setAcceptor(IAcceptor &acceptor)
{
    Acceptor    =   &acceptor;
}

void CUrlNormalizer::pushLink(const std::string& link)
{
    Storage << "\t" << link  << std::endl;

    if (  (prefix::isMail(link))
       || (prefix::isJavaScript(link))
       || (prefix::isHttps(link)) ) {
        return;
    }

    if (0 == ParentLink) {
        Acceptor->pushLink(CLink(link));
    }
    else {
        Acceptor->pushLink(CLink(*ParentLink, link));
    }

}

void CUrlNormalizer::setFrom(const CLink &uri)
{
    Storage << uri.toString() << std::endl;
    ParentLink  =   &uri;
}
