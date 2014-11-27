#ifndef _LINK_BUFFER_H_
#define _LINK_BUFFER_H_
#include <string>
#include <set>
#include <list>

#include <boost/unordered_set.hpp>

#include "whale_link.h"
#include "ilink_buffer.h"




class CLinkBuffer: public ILinkBuffer{
public:
    bool                pop(CLink &top);
    void                pushLink(CLink const &link);

    size_t              size()const;
    bool                empty()const;

    bool                IsFutureEmpty()const;
    bool                IsUsedEmpty()const;

    void                writeUsedLinks(std::ostream &uo)const;
    void                writeFutureLinks(std::ostream &fo)const;
    void                readUsedLinks(std::istream &ui);
    void                readFutureLinks(std::istream &fi);


private:

    std::list<CLink>	m_futureLinks;
    boost::unordered_set<CLink, ihash>
                        m_usedLinks;
};
#endif //_LINK_BUFFER_H_
