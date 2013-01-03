#pragma once

#include <vector>
#include <string>

#include "memory_repr_traits.h"
#include "base_mappers.h"

namespace korm {

/*! \brief 
 *         Map to binary representation plain data types
 */

template<class T>
class CMapper<T, kPodType> {
public:
    CMapper( const T& opd )
    : m_pPtr((const char*)(&opd))
    {}

    const char* dataPointer() const
    {
        return m_pPtr;
    }

    size_t dataSize() const
    {
        return sizeof(T);
    }

private:
    const char*	m_pPtr;
};

/*! \brief 
 *         Unmap plain types from binary representation 
 */
template<class T>
class CUnMapper<T, kPodType> {
public:
    CUnMapper( T& target )
    : m_tData(target)
    {}

    void unMap( const char *dataPtr, size_t dataSize )
    {
        memcpy(&m_tData, dataPtr, dataSize);
    }

private:
    T&  m_tData;
};

/*! \brief 
 *         Map vector of plain types
 */

template<class T>
class CMapper<std::vector<T>, kPodType > {
public:
    typedef std::vector<T>  CVector;

    CMapper( const CVector& vpd )
    : m_pPtr((const char*)(&vpd[0]))
    , m_iSize(sizeof(T) * vpd.size())
    {}

    const char* dataPointer() const
    {
        return m_pPtr;
    }

    size_t dataSize() const
    {
        return m_iSize;
    }

private:
    const char*	m_pPtr;
    size_t      m_iSize;
};

/*! \brief 
 *         Unmap vector of plain types
 */

template<class T>
class CUnMapper<std::vector<T>, kPodType > {
public:

    typedef std::vector<T>  CVector;

    CUnMapper( CVector& target )
    : m_vData(target)
    {}

    void unMap( const char *dataPtr, size_t dataSize )
    {
        m_vData.resize(dataSize / sizeof(T));
        memcpy(&m_vData[0], dataPtr, dataSize);
    }

private:
    CVector&				m_vData;
};

/*! \brief 
 *         map standard strings (std::string, std::wstring)
 */

template<typename _CharT, typename _Traits, typename _Alloc>
class CMapper<std::basic_string<_CharT, _Traits, _Alloc>, kPodType> {
public:
    typedef std::basic_string<_CharT, _Traits, _Alloc>  CString;

    CMapper( const CString& spd )
    : m_pPtr((const char*)(spd.c_str()))
    , m_iSize(sizeof(_CharT) * spd.size())
    {}

    const char* dataPointer() const
    {
        return m_pPtr;
    }

    size_t dataSize() const
    {
        return m_iSize;
    }

private:
    const char*	m_pPtr;
    size_t          m_iSize;
};

/*! \brief 
 *         Unmap standard strings (std::string, std::wstring)
 */

template<typename _CharT, typename _Traits, typename _Alloc>
class CUnMapper<std::basic_string<_CharT, _Traits, _Alloc>, kPodType> {
public:
    typedef std::basic_string<_CharT, _Traits, _Alloc>  CString;

    CUnMapper( CString& target )
    : m_vData(target)
    {}

    void unMap( const char *dataPtr, size_t dataSize )
    {
        m_vData.resize(dataSize / sizeof(_CharT));
        memcpy(&m_vData[0], dataPtr, dataSize);
    }

private:
    CString&    m_vData;
};





}//korm - kyoto orm
