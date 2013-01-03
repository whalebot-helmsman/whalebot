#pragma once

#include "memory_repr_traits.h"
#include "size_traits.h"
#include "base_writers.h"

namespace korm {
template<class T>
class CWritingTraits {
public:
public:
    typedef CBinWriter<T, CMemoryReprTraits<T>::eIsPod, CWriteSizeTraits<T>::eIsWrite>  CDefaultWriter;
    typedef CBinReader<T, CMemoryReprTraits<T>::eIsPod, CWriteSizeTraits<T>::eIsWrite>  CDefaultReader;

};
}//korm - kyoto orm
