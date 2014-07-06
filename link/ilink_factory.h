/*
 * File:   ilink_factory.h
 * Author: hotdox
 *
 * Created on May 1, 2010, 11:26 AM
 */

#ifndef _ILINK_FACTORY_H
#define	_ILINK_FACTORY_H

#include "iacceptor.h"

class CLink;

class ILinkFactory {
public:
    virtual void    pushLink(std::string link) = 0;
    virtual void    setFrom(CLink const &uri) = 0;
    virtual void    setAcceptor(IAcceptor &acceptor) = 0;
    virtual         ~ILinkFactory(){}
};



#endif	/* _ILINK_FACTORY_H */

