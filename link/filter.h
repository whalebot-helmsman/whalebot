/*
 * File:   Filter.h
 * Author: hotdox
 *
 * Created on February 23, 2009, 9:39 PM
 */

#ifndef _FILTER_H
#define	_FILTER_H
#include "whale_link.h"

class IFilter{
public:
    virtual bool filtrate(CLink const &link) =0;
    virtual ~IFilter(){}
};



#endif	/* _FILTER_H */

