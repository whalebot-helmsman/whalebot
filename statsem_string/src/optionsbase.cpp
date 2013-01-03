#include "optionsbase.h"
#include "streamparser.h"
#include "optionsparser.h"


void COptionsBase::clear(){
    map.clear();
}
void COptionsBase::set(std::string const &key, std::string const &value){
    options_map_t::iterator i(map.lower_bound(key));
    if((i != map.end())&&(i->first == key)){
        i->second   =   value;
    }else{
        map.insert(i, std::make_pair(key, value));
    }
}
options_map_t::const_iterator  COptionsBase::cbegin()const{
    return map.begin();
}
options_map_t::const_iterator  COptionsBase::cend()const{
    return map.end();
}
bool COptionsBase::get(std::string const &key, std::string &value)const{
    bool                            ret(false);
    options_map_t::const_iterator   i(map.find(key));
    if(i != map.end()){
        value   =   i->second;
        ret     =   true;
    }
    return ret;
}
void COptionsBase::read(std::istream &in){
    COptionsParser  parser(*this);
    CStreamParserEngine::parseFile(in, parser);
}
void COptionsBase::write(std::ostream &out)const{
    for(options_map_t::const_iterator i = map.begin(); i != map.end(); ++i){
        out<<i->first<<"\t=\t"<<i->second<<std::endl;
    }
}
COptionsBase::~COptionsBase(){
}
