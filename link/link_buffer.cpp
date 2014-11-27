#include <iostream>
#include "filter.h"
#include "link_buffer.h"


void CLinkBuffer::writeUsedLinks(std::ostream &uo)const{
    for(boost::unordered_set<CLink>::const_iterator i = m_usedLinks.begin();
        i != m_usedLinks.end(); ++i){
        uo<<*i;
    }
}

void CLinkBuffer::writeFutureLinks(std::ostream &fo)const{
    for(std::list<CLink>::const_iterator i = m_futureLinks.begin();
        i != m_futureLinks.end(); ++i){
        fo<<*i;
    }
}
void CLinkBuffer::readFutureLinks(std::istream &ui){
    CLink    buff;
    while(ui>>buff){
        m_futureLinks.push_back(buff);
    }
}
void CLinkBuffer::readUsedLinks(std::istream &fi){
    CLink    buff;
    while(fi>>buff){
        m_usedLinks.insert(buff);
    }

}

bool CLinkBuffer::pop(CLink &top){
    bool    ret(false);
    if(!m_futureLinks.empty()){
        ret =   true;
        top =   m_futureLinks.front();
        m_futureLinks.pop_front();
    }
    return ret;
}

size_t CLinkBuffer::size()const{
    return m_futureLinks.size();
}


bool CLinkBuffer::empty()const{
    return m_futureLinks.empty();
}


bool CLinkBuffer::IsFutureEmpty()const
{
    return m_futureLinks.empty();
}

bool CLinkBuffer::IsUsedEmpty()const
{
    return m_usedLinks.empty();
}

void CLinkBuffer::pushLink(CLink const &link){
    if(m_usedLinks.find(link) == m_usedLinks.end()){
        m_usedLinks.insert(link);
        m_futureLinks.push_back(link);
    }
}



