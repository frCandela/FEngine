#include "fanGlobalIncludes.h"
#include "ecs/fanEcsComponentsKey.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ecsComponentsKeyCompact::ecsComponentsKeyCompact()
	{
		Reset();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsComponentsKeyCompact::Reset()
	{
		for ( int elemIndex = 0; elemIndex < s_maxComponentsPerEntity; elemIndex++ )
		{
			chunck[elemIndex] = 0;
			element[elemIndex] = 0;
		}

		bitset = ecsBitset( 0 );
		m_componentsKeys = ~indicesBitset( 0 );
		m_nextElement = 0;
	}

	//================================================================================================================================
	// Returns the index of a component "_index" in the chunck&element arrays
	//================================================================================================================================
	uint32_t ecsComponentsKeyCompact::GetIndex( const uint32_t _index ) const
	{
		return  (uint32_t)( ( m_componentsKeys >> ( _index * s_indexWidth ) ) & indicesBitset ( s_clearMask ) ).data()[0];
	}

	//================================================================================================================================
	// Set the key at index "_index" to point to a specific "_value" which is an index in the chunck&element arrays
	//================================================================================================================================
	void ecsComponentsKeyCompact::SetKey( const uint32_t _index, const uint32_t _value )
	{
		indicesBitset valueMask = indicesBitset( _value ) << ( _index * s_indexWidth );
		indicesBitset clearMask = ~( indicesBitset( s_clearMask ) << ( _index * s_indexWidth ) );

		m_componentsKeys &= clearMask;
		m_componentsKeys |= valueMask;
	}

	//================================================================================================================================
	// Sets the chunck&element indices for the component at index "_index"
	// Makes the key at index "_index" point towards a new component index in the chunck&element arrays
	//================================================================================================================================
	void ecsComponentsKeyCompact::SetComponent( const uint32_t _index, const uint16_t _chunckIndex, const uint16_t _elementIndex )
	{
		assert( m_nextElement < s_maxComponentsPerEntity );
		assert( GetIndex( _index ) == s_clearMask );

		chunck[m_nextElement] = _chunckIndex;
		element[m_nextElement] = _elementIndex;

		SetKey( _index, m_nextElement );

		++m_nextElement;
	}
}