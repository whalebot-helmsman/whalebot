#pragma once
#include "memory_repr_traits.h"
#include "size_traits.h"
#include "mapping_traits.h"
#include "writing_traits.h"

namespace korm {

/*! \brief
 *        Write pod types without size prefix
 */
template<class T>
class CBinWriter<T, kPodType, kNonWrite> {
public:
    typedef typename CMappingTraits<T>::CDefaultMapper  CMapper;


    CBinWriter(const T& pod)
    : m_tMapper(pod)
    {}

    char* write(char* place) {
        memcpy(place, m_tMapper.dataPointer(), m_tMapper.dataSize());
        return (place + m_tMapper.dataSize());
    }

private:
    CMapper m_tMapper;
};

/*! \brief
 *          Read pod types without size
 */

template<class T>
class CBinReader<T, kPodType, kNonWrite>{
public:
    typedef typename CMappingTraits<T>::CDefaultUnMapper  CUnMapper;


    CBinReader(T& pod)
    : m_tUnMapper(pod)
    {}

    char* read(char* place) {
        m_tUnMapper.unMap(place, sizeof(T));
        return (place + sizeof(T));
    }

private:
    CUnMapper m_tUnMapper;
};

/*! \brief
 *        Write pod types with size prefix
 */
template<class T>
class CBinWriter<T, kPodType, kWrite> {
public:
    typedef typename CMappingTraits<T>::CDefaultMapper       CMapper;
    typedef typename CWritingTraits<size_t>::CDefaultWriter  CSizeWriter;


    CBinWriter(const T& pod)
    : m_tMapper(pod)
    {}

    char* write(char* place) {
        char*       iter(place);
        CSizeWriter sizeWriter(m_tMapper.dataSize());
        iter    =   sizeWriter.write(iter);
        memcpy(iter, m_tMapper.dataPointer(), m_tMapper.dataSize());
        return (iter + m_tMapper.dataSize());
    }

private:
    CMapper m_tMapper;
};

/*! \brief
 *          Read pod types with size
 */

template<class T>
class CBinReader<T, kPodType, kWrite>{
public:
    typedef typename CMappingTraits<T>::CDefaultUnMapper    CUnMapper;
    typedef typename CWritingTraits<size_t>::CDefaultReader CSizeReader;


    CBinReader(T& pod)
    : m_tUnMapper(pod)
    {}

    char* read(char* place) {
        char*       iter(place);
        size_t      dataSize;
        CSizeReader sizeReader(dataSize);
        iter    =   sizeReader.read(iter);
        m_tUnMapper.unMap(iter, dataSize);
        return (iter + dataSize);
    }

private:
    CUnMapper m_tUnMapper;
};


}//korm - kyoto orm
