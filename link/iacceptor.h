/* 
 * File:   iacceptor.h
 * Author: hotdox
 *
 * Created on September 27, 2009, 7:00 AM
 */

#ifndef _IACCEPTOR_H
#define	_IACCEPTOR_H

class CLink;

class IAcceptor{
public:
    virtual void    pushLink(CLink const &link) = 0;
    virtual         ~IAcceptor(){}
};



#endif	/* _IACCEPTOR_H */

