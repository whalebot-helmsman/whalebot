/* 
 * File:   LinkFactory.h
 * Author: hotdox
 *
 * Created on September 19, 2009, 2:37 PM
 */

#ifndef _LINKFACTORY_H
#define	_LINKFACTORY_H

#include <string>

#include <ilink_factory.h>
#include <whale_link.h>



class IAcceptor;


class CLinkFactory : public ILinkFactory {
public:
                CLinkFactory();
    void        pushLink(std::string link);
    void        setFrom(CLink const &uri);
    void        setAcceptor(IAcceptor &acceptor);


private:
    const CLink*    m_tParentLink;
    IAcceptor*      m_pAcceptor;
};



#endif	/* _LINKFACTORY_H */

