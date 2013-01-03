#ifndef PARSERBASE_H
#define PARSERBASE_H

#include <string>
#include <vector>


class CParserBase{
public:
    typedef std::string::const_iterator     str_ptr_t;
    typedef std::pair<str_ptr_t, str_ptr_t> str_interval_t;
    typedef std::vector<str_interval_t>     poser_t;
    typedef poser_t::const_iterator         poser_ptr_t;

                CParserBase(char delimiter);
                CParserBase(std::string const &delimiters);
                ~CParserBase();

    std::string getDelimiter()const;
    void        setDelimiter(std::string const &delimiters);

    void        parseString(std::string const &str);

    size_t      size()const;

    bool        getPart(size_t start, size_t stop, std::string &result);
    bool        getTail(size_t start, std::string &result);
    bool        getStart(size_t stop, std::string &result);
    bool        getElement(size_t i, std::string &result);

    std::string unsafe_getTail(size_t start);    
    std::string unsafe_getStart(size_t stop);
    std::string unsafe_getPart(size_t start, size_t stop);
    std::string unsafe_getElement(size_t i);

protected:
    std::string         delimiters;
    std::string const   *str;
    poser_t		poses;
};


#endif // PARSERBASE_H
