/*
 * File:   OneServerFilter.h
 * Author: hotdox
 *
 * Created on February 23, 2009, 9:46 PM
 */

#ifndef _FILTERS_H
#define	_FILTERS_H

#include "filter.h"
#include "iacceptor.h"

#include <options/options.hpp>

#include <list>
#include <iostream>

class CAllFilter: public IFilter{
public:
    bool    filtrate(CLink const &link);
};

class COneServerFilter: public IFilter{
public:
            COneServerFilter();
    bool    filtrate(CLink const &link);
private:
    std::string m_server;
};

class CIncludeFilter: public IFilter{
public:
            CIncludeFilter(std::istream &in);
            CIncludeFilter(const CLinkFilterOptions::CWordList& requiredWords);
    void    addWord(const std::string &incl);
    bool    filtrate(CLink const &link);
private:
    std::list<std::string>  m_includeWords;
};

class CLevelFilter: public IFilter{
public:
            CLevelFilter(size_t level);
    bool    filtrate(CLink const &link);
private:
    size_t  m_level;
};

class CFilterList: public IFilter, public IAcceptor{
public:
            CFilterList();
    void    setAcceptor(IAcceptor &acceptor);
    bool    filtrate(CLink const &link);
    void    pushLink(CLink const &link);
    void    addFilter(IFilter *filter);
            ~CFilterList();
private:
    std::list<IFilter *>    m_storage;
    IAcceptor               *m_pAcceptor;
};

#endif	/* _FILTERS_H */

