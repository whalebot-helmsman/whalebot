#include <statsem_string/include/parserbase.h>
#include "filters.h"



bool    CAllFilter::filtrate(CLink const &link){
    return true;
}
COneServerFilter::COneServerFilter()
:m_server(""){}
bool    COneServerFilter::filtrate(CLink const &link){
    bool    ret(true);
    if(m_server.empty()){
        m_server    =   link.getServer();
    }else{
        ret     =   (m_server == link.getServer());
    }
    return ret;
}
void CIncludeFilter::addWord(const std::string &incl){
    this->m_includeWords.push_back(incl);
}
CIncludeFilter::CIncludeFilter(std::istream &in){
    std::string tmp;
    while(in>>tmp){
        m_includeWords.push_back(tmp);
    }
}
bool CIncludeFilter::filtrate(CLink const &link){
    bool    ret(true);
    if(!m_includeWords.empty()){
        ret =   false;
        std::list<std::string>::iterator i = m_includeWords.begin();
        while((i != m_includeWords.end())&&(!ret)){
            ret |=  link.getUri().find(*i) != std::string::npos;
            ret |=  link.getServer().find(*i) != std::string::npos;
            ++i;
        }
    }
    return ret;
}


void CFilterList::setAcceptor(IAcceptor &acceptor){
    m_pAcceptor =   &acceptor;
}
bool CFilterList::filtrate(CLink const &link){
    bool    ret(true);
    if(!m_storage.empty()){
        std::list<IFilter *>::iterator  i(m_storage.begin());
        while((ret)&&(i != m_storage.end())){
            ret =   (*i)->filtrate(link);
            ++i;
        }
    }
    return ret;
}

CFilterList::CFilterList()
:m_pAcceptor(0){}
void CFilterList::pushLink(CLink const &link){
    if(filtrate(link)){
        m_pAcceptor->pushLink(link);
    }
}
void CFilterList::addFilter(IFilter *filter){
    if(filter){
        m_storage.push_back(filter);
    }
}
CFilterList::~CFilterList(){
    for(std::list<IFilter *>::iterator i = m_storage.begin(); i != m_storage.end(); ++i){
        delete *i;
    }
    m_storage.clear();
}


CLevelFilter::CLevelFilter(size_t level)
:m_level(level){}

bool CLevelFilter::filtrate(CLink const &link){
    CParserBase parser("/");
    parser.parseString(link.getUri());
    return (parser.size() < m_level);
}
