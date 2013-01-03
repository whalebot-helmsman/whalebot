/* 
 * File:   HeaderParser.h
 * Author: hotdox
 *
 * Created on February 22, 2009, 2:35 PM
 */

#ifndef _HEADERPARSER_H
#define	_HEADERPARSER_H
#include <string>
#include <map>

typedef std::map<std::string,std::string>    header_map;

class CHeaderParser{
public:
            //CHeaderParser(const std::string &header);
    void    addString(const std::string &str);
    bool    getField(const std::string &field, std::string &value)const;
    bool    getExtension(std::string &ext);
    bool    getCookies(std::string &cookies);
    header_map::const_iterator
            begin()const;
    header_map::const_iterator
            end()const;
    void    clear();
    size_t  size();
private:
    header_map  m_fields;
};



#endif	/* _HEADERPARSER_H */

