#include <fstream>
#include <to_string.h>

const std::string   kTrueStr    =   "true";
const std::string   kFalseStr   =   "false";

template<>
std::string toString(std::string const &x){
    return x;
}

template<>
std::string toString(bool const &x){
    if (true == x) {
        return kTrueStr;
    }
    else {
        return kFalseStr;
    }
}

void trim(std::string &where, std::string const &what){
    std::string::iterator           begin(inner_cut(where.begin(), where.end(), what));
    std::string::iterator           end(inner_cut(where.rbegin(), where.rend(), what).base());
    if(begin < end){
        where.assign(begin, end);
    }else{
        where.clear();
    }
}


template<>
bool fromString(std::string const &x,std::string  &y){
    y = x;
    return true;
}

template<>
bool fromString(std::string const &x,bool  &y){
    if (kTrueStr == x) {
        y   =   true;
        return false;        
    }
    else if (kFalseStr == x) {
        y   =   false;
        return true;
    }
    else {
        return false;
    }
}

void copyStream(std::istream &from , std::ostream &to){
    to << from.rdbuf();
}

