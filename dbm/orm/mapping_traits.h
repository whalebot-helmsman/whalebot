#pragma once

#include "base_mappers.h"
#include "memory_repr_traits.h"

namespace korm {

/*! \brief
 *         Class for automatic detection of mapper/unmapper
 */

template<class T>
class CMappingTraits {
public:
    typedef CMapper<T, CMemoryReprTraits<T>::eIsPod>    CDefaultMapper;
    typedef CUnMapper<T, CMemoryReprTraits<T>::eIsPod>  CDefaultUnMapper;
};

}//korm - kyoto orm
