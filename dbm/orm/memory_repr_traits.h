#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>
#include <list>
#include <deque>

namespace korm {

const unsigned int kPodType     =   1;
const unsigned int kNonPodType  =   0;

template<class T>
struct CMemoryReprTraits {
    enum { eIsPod          = kPodType };
    enum { eIsInclusivePod = kPodType };
};

template<class T>
struct CMemoryReprTraits<T*> {
    enum { eIsPod          = CMemoryReprTraits<T>::eIsPod };
    enum { eIsInclusivePod = kNonPodType };
};

template<class T>
struct CMemoryReprTraits<std::vector<T> > {
    enum { eIsPod          = CMemoryReprTraits<T>::eIsInclusivePod };
    enum { eIsInclusivePod = kNonPodType };
};

template<class T>
struct CMemoryReprTraits<std::list<T> > {
    enum { eIsPod          = kNonPodType };
    enum { eIsInclusivePod = kNonPodType };
};

template<class KT, class VT>
struct CMemoryReprTraits<std::map<KT,VT> > {
    enum { eIsPod          = kNonPodType };
    enum { eIsInclusivePod = kNonPodType };
};

template<class T>
struct CMemoryReprTraits<std::set<T> > {
    enum { eIsPod          = kNonPodType };
    enum { eIsInclusivePod = kNonPodType };
};

template<class T>
struct CMemoryReprTraits<std::deque<T> > {
    enum { eIsPod          = kNonPodType };
    enum { eIsInclusivePod = kNonPodType };
};

template<typename _CharT, typename _Traits, typename _Alloc>
struct CMemoryReprTraits<std::basic_string<_CharT, _Traits, _Alloc> > {
    enum { eIsPod          = kPodType };
    enum { eIsInclusivePod = kNonPodType };
};

template<class FT, class ST>
struct CMemoryReprTraits<std::pair<FT, ST> > {
    enum { eIsPod          = CMemoryReprTraits<FT>::eIsInclusivePod & CMemoryReprTraits<ST>::eIsInclusivePod };
    enum { eIsInclusivePod = eIsPod };
};

}//korm - kyoto orm
