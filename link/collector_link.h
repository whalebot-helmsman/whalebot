/*
 * File:   collector_link.h
 * Author: hotdox
 *
 * Created on May 1, 2010, 11:14 AM
 */

#ifndef _COLLECTOR_LINK_H
#define	_COLLECTOR_LINK_H

#include <fstream>
#include "link_factory.h"
#include "ilink_factory.h"

class CCollectorLinkFactory : public ILinkFactory {
public:
                CCollectorLinkFactory();
    void        pushLink(std::string link);
    void        setFrom(CLink const &uri);
    void        setAcceptor(IAcceptor &acceptor);
private:
    CLinkFactory    m_Base;
    std::ofstream   m_File;
};



#endif	/* _COLLECTOR_LINK_H */

