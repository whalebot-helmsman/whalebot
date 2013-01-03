#ifndef OPTIONSBASE_H
#define OPTIONSBASE_H

#include <map>
#include <string>
#include <iostream>

typedef std::map<std::string, std::string>  options_map_t;

class COptionsBase {
public:
    options_map_t::const_iterator   cbegin()const;
    options_map_t::const_iterator   cend()const;

    void    clear();
    void    set(std::string const &key, std::string const &value);
    bool    get(std::string const &key, std::string &value)const;
    void    read(std::istream &in);
    void    write(std::ostream &out)const;
            ~COptionsBase();
protected:
    options_map_t   map;
};

#endif // OPTIONSBASE_H
