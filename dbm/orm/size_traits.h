#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>
#include <list>
#include <deque>

namespace korm {

const unsigned int kWrite     =   1;
const unsigned int kNonWrite  =   0;

template<class T>
struct CWriteSizeTraits {
    enum { eIsWrite = kNonWrite };
};

template<class T>
struct CWriteSizeTraits<T*> {
    enum { eIsWrite = CWriteSizeTraits<T>::eIsWrite };
};

template<class T>
struct CWriteSizeTraits<std::vector<T> > {
    enum { eIsWrite = kWrite };
};

template<class T>
struct CWriteSizeTraits<std::list<T> > {
    enum { eIsWrite = kWrite };
};

template<class KT, class VT>
struct CWriteSizeTraits<std::map<KT,VT> > {
    enum { eIsWrite = kWrite };
};

template<class T>
struct CWriteSizeTraits<std::set<T> > {
    enum { eIsWrite = kWrite };
};

template<class T>
struct CWriteSizeTraits<std::deque<T> > {
    enum { eIsWrite = kWrite };
};

template<typename _CharT, typename _Traits, typename _Alloc>
struct CWriteSizeTraits<std::basic_string<_CharT, _Traits, _Alloc> > {
    enum { eIsWrite = kWrite };
};

template<class FT, class ST>
struct CWriteSizeTraits<std::pair<FT, ST> > {
    enum { eIsWrite = (CWriteSizeTraits<FT>::eIsWrite | CWriteSizeTraits<ST>::eIsWrite) };
};

}//korm - kyoto orm

