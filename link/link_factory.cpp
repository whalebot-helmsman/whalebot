#include <set>
#include <iostream>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

#include "link_factory.h"
#include "link.h"
#include "prefix.h"
#include "ilink_buffer.h"


CLinkFactory::CLinkFactory()
: m_tParentLink(0)
, m_pAcceptor(0)
{}

void CLinkFactory::setAcceptor(IAcceptor &acceptor){
    m_pAcceptor  =   &acceptor;
}

void CLinkFactory::pushLink(std::string link) {

    if((prefix::isMail(link))||(prefix::isJavaScript(link))||(prefix::isHttps(link)))
        return;

    if (0 == m_tParentLink) {
        m_pAcceptor->pushLink(CLink(link));
    }
    else {
        m_pAcceptor->pushLink(CLink(*m_tParentLink, link));
    }
}

void CLinkFactory::setFrom(CLink const &uri){
    m_tParentLink   =   &uri;
}
