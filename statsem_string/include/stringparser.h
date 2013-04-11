#ifndef STRINGPARSER_H
#define STRINGPARSER_H


#include "parserbase.h"

class CStringParser : public CParserBase{
public:

    CStringParser(char delimiter)
            :CParserBase(delimiter){}
    CStringParser(std::string const &delimiters)
            :CParserBase(delimiters){}

    template<class T>bool unsafe_getElement(T &x, size_t i = 0){

        return fromString(CParserBase::unsafe_getElement(i), x);
    }

    template<class T>bool getElement(T &x, size_t i = 0){
        bool        ret =   false;
        std::string tmp;

        if(CParserBase::getElement(i, tmp)){
            ret =   fromString(tmp, x);

        }

        return ret;
    }

    template<class T>bool unsafe_getElementFromEnd(T &x, size_t i = 0){
            return unsafe_getElement(x, poses.size() - 1 - i);
    }

    template<class T>bool getElementFromEnd(T &x, size_t i = 0){
            return getElement(x, poses.size() - 1 - i);
    }
    ~CStringParser(){}
};

#endif // STRINGPARSER_H
