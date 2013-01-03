#ifndef OPTIONS_H
#define OPTIONS_H


#include "optionsbase.h"
#include "to_string.h"


class  COptions : public COptionsBase{
public:
    template<class T> bool    get(std::string const &key, T &x)const{
        std::string value("");
        bool        ret(false);
        ret =   (COptionsBase::get(key, value))&&(fromString(value, x));
        return ret;
    }

    template<class T> void    set(std::string const &key, T x){
        COptionsBase::set(key, toString(x));
    }
};

#endif // OPTIONS_H
