#include "fanGlobalIncludes.h"

#include "core/fanTypeInfo.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	uint32_t TypeInfo::Register( const uint32_t _key, std::function<void*( )> _constructor, const uint32_t _flags  )
	{
		assert( m_data.find( _key ) == m_data.end() );

		TypeInfoData data;
		data.constructor = _constructor;
		data.instance = _constructor();
		data.flags = _flags;
		m_data[_key] = data;

		return _key;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector< const void * > TypeInfo::GetInstancesWithFlags( const uint32_t _flags)
	{
		std::vector< const void * > instances;
		for ( auto dataPair : m_data )
		{
			TypeInfoData & data = dataPair.second;
			if ( (data.flags & _flags) == _flags )
			{
				instances.push_back( data.instance );
			}
		}
		return instances;
	}
}