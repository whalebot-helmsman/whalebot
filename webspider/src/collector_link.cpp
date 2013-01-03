#include "collector_link.h"

CCollectorLinkFactory::CCollectorLinkFactory()
:m_Base()
{
    m_File.open("rl_links.txt", std::ios_base::app);
}

void CCollectorLinkFactory::pushLink(std::string link)
{
    m_File << "\t" << link  << std::endl;
    m_Base.pushLink(link);
}

void CCollectorLinkFactory::setFrom(CLink const &uri)
{
    m_File << uri.toString() << std::endl;
    m_Base.setFrom(uri);
}

void CCollectorLinkFactory::setAcceptor(IAcceptor &acceptor)
{
    m_Base.setAcceptor(acceptor);
}
