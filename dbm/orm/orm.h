#pragma once

namespace korm {

template< class DB
        , class KT
        , class VT
        , class KMapper = CMappingTraits<KT>::CDefaultMapper
        , class VMapper = CMappingTraits<VT>::CDefaultMapper >
bool set ( DB& db, const KT& key, const VT& value)
{
	KMapper	keyMapper(key);
	VMapper valueMapper(value);
	      
	return db.set( keyMapper.dataPointer()
				  , keyMapper.dataSize()
	              , valueMapper.dataPointer()
	              , valueMapper.dataSize());
}

template< class DB
        , class KT
        , class VT
        , class KMapper = CMappingTraits<KT>::CDefaultMapper
        , class VUnMapper = CMappingTraits<VT>::CDefaultUnMapper >
bool get ( DB& db, const KT& key, VT& value)
{
	KMapper	keyMapper(key);
	size_t	valueSize(0);
	char*	valuePtr(0);
	
	valuePtr	=	db.get( keyMapper.dataPointer()
					      , keyMapper.dataSize()
						  , *valueSize);

    if (0 == valuePtr) {
		return false;
	}

	VUnMapper	gotValue(value);
	gotValue.unMap(valuePtr, valueSize);

	delete[] valuePtr;

    return true;						  
}


}//korm - kyoto orm
