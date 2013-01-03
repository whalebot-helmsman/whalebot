#include "parserbase.h"

CParserBase::CParserBase(char delimiter)
:str(0){
        setDelimiter(std::string(1, delimiter));
}
CParserBase::CParserBase(std::string const &delimiters)
:str(0){
    setDelimiter(delimiters);
}
std::string CParserBase::getDelimiter()const{
    return delimiters;
}
CParserBase::~CParserBase(){}
void CParserBase::setDelimiter(std::string const &delimiters){
    this->delimiters	=	delimiters;
    str                 =   0;
    poses.clear();
}

void CParserBase::parseString(std::string const &str){
    this->str	=	&str;
    poses.clear();

    str_ptr_t   i(this->str->begin()),
                j(i);

    while(i != this->str->end()){

        //skip delimiters
        while((i != this->str->end())&&(delimiters.find(*i) != std::string::npos))
        ++i;

        j   =   i;

    //collect signs
    while((i != this->str->end())&&(delimiters.find(*i) == std::string::npos))
        ++i;

    if(std::distance(j, i) > 0)
        poses.push_back(std::make_pair(j, i));
    }
}
size_t CParserBase::size()const{
        return poses.size();
}
bool CParserBase::getElement(size_t i, std::string &result){
    if(!str)
        return false;

    if(poses.empty())
        return false;

    if(poses.size() <= i)
        return false;

    result  =   unsafe_getElement(i);

    return true;
}
bool CParserBase::getPart(size_t start, size_t stop, std::string &result){
    if(!str)
        return false;

    if(poses.empty())
        return false;

    if((poses.size() <= start)||(poses.size() <= stop))
                return false;

    result  =   unsafe_getPart(start, stop);
    return true;
}
std::string CParserBase::unsafe_getElement(size_t i){
    poser_ptr_t p(poses.begin());
    std::advance(p, i);
    return std::string(p->first, p->second);
}
std::string CParserBase::unsafe_getTail(size_t start){
    poser_ptr_t	j(poses.begin());

        std::advance(j,start);


        str_ptr_t   p(j->first),
                    p1(str->end());


    return std::string(p,p1);
}
bool CParserBase::getTail(size_t start, std::string &result){
    if(!str)
        return false;

    if(poses.size() <= start)
        return false;

    result  =   unsafe_getTail(start);

    return true;
}
bool CParserBase::getStart(size_t stop, std::string &result){
    if(!str)
        return false;

    if(poses.size() <= stop)
        return false;

    result  =   unsafe_getStart(stop);

    return true;
}
std::string CParserBase::unsafe_getStart(size_t stop) {
    poser_ptr_t j1(poses.begin());
    std::advance(j1, stop);

    str_ptr_t p(str->begin()),
            p1(j1->second);


    return std::string(p, p1);
}
std::string CParserBase::unsafe_getPart(size_t start, size_t stop){
    poser_ptr_t	j(poses.begin()),
            j1(poses.begin());

        std::advance(j,start);
        std::advance(j1,stop);

        str_ptr_t   p(j->first),
                    p1(j1->first);


    return std::string(p,p1);
}
