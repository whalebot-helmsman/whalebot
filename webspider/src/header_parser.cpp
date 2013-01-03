#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <parserbase.h>

#include <header_parser.h>



bool CHeaderParser::getField(const std::string &field, std::string &value)const{
    bool                        ret(false);
    header_map::const_iterator  i(m_fields.find(field));
    if(i != m_fields.end()){
        ret     =   true;
        value   =   i->second;
    }
    return ret;
}
bool CHeaderParser::getCookies(std::string &cookies){

    return getField("Set-Cookie",cookies);

//    if(!getField("Set-Cookie",cookies))
//            return false;
//
//    bool            ret(true);
//    std::string     cookie_name,
//                    cookie_body;
//    CStringParser   parser(";"),
//                    inner_parser("=");
//    parser.parseString(cookies);
//    ret &=  parser.unsafe_getElement(cookies);
//    inner_parser.parseString(cookies);
//    ret &=  inner_parser.getElement(cookie_name);
//    ret &=  inner_parser.getElementFromEnd(cookie_body);
//    boost::to_lower(cookie_name);
//    if(ret){
//        cookies =   cookie_name + "=" + cookie_body;
//    }
//    return ret;
}
bool CHeaderParser::getExtension(std::string &ext){
    bool    ret(true);
    
    if(!getField("Content-Type", ext))
        return false;

    CParserBase parser(";"),
                inner_parser("/");

    parser.parseString(ext);
    ret =   parser.getElement(0, ext);
    inner_parser.parseString(ext);
    inner_parser.getTail(1, ext);
    return ret;
}
void CHeaderParser::addString(const std::string &str){
    std::string field,
                value;

    CParserBase   parser(":");
    parser.parseString(str);


    parser.getElement(0, field);
    if(!parser.getTail(1, value)){
        value   =   str;
        field   =   boost::lexical_cast<std::string>(m_fields.size());
    }

    if(!value.empty()){
        boost::trim(field);
        boost::trim(value);
        m_fields.insert(std::make_pair(field,value));
    }
}
size_t CHeaderParser::size(){
    return m_fields.size();
}
void CHeaderParser::clear(){
    m_fields.clear();
}
header_map::const_iterator CHeaderParser::begin()const{
    return m_fields.begin();
}
header_map::const_iterator CHeaderParser::end()const{
    return m_fields.end();
}

