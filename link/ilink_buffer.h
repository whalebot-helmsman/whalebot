/*
 * File:   ilinkbuffer.h
 * Author: hotdox
 *
 * Created on September 19, 2009, 2:19 PM
 */

#ifndef _ILINK_BUFFER_H
#define	_ILINK_BUFFER_H

#include "iacceptor.h"

class ILinkBuffer: public IAcceptor{
public:
    virtual bool    pop(CLink &top) = 0;

    virtual size_t  size()const = 0;
    virtual bool    empty()const = 0;

    virtual void    writeUsedLinks(std::ostream &uo)const = 0;
    virtual void    writeFutureLinks(std::ostream &fo)const = 0;
    virtual void    readUsedLinks(std::istream &ui) = 0;
    virtual void    readFutureLinks(std::istream &fi) = 0;
    virtual         ~ILinkBuffer(){}
};


#endif	/* _ILINK_BUFFER_H */

