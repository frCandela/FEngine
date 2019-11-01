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

		m_bitset = ecsBitset( 0 );
		m_componentsKeys = ~indicesBitset( 0 );
		m_nextElement = 0;
	}

	//================================================================================================================================
	// Returns the index of a component "_index" in the chunck&element arrays
	//================================================================================================================================
	uint32_t ecsComponentsKeyCompact::GetIndex( const uint32_t _componentIndex ) const
	{
		return  (uint32_t)( ( m_componentsKeys >> ( _componentIndex * s_indexWidth ) ) & indicesBitset ( s_emptyKeyValue ) ).data()[0];
	}

	//================================================================================================================================
	// Set the key at index "_index" to point to a specific "_value" which is an index in the chunck&element arrays
	//================================================================================================================================
	void ecsComponentsKeyCompact::SetIndex( const uint32_t _componentIndex, const uint32_t _value )
	{
		indicesBitset valueMask = indicesBitset( _value ) << ( _componentIndex * s_indexWidth );
		indicesBitset clearMask = ~( indicesBitset( s_emptyKeyValue ) << ( _componentIndex * s_indexWidth ) );

		m_componentsKeys &= clearMask;
		m_componentsKeys |= valueMask;
	}

	//================================================================================================================================
	// Sets the chunck&element indices for the component at index "_index"
	// Makes the key at index "_index" point towards a new component index in the chunck&element arrays
	//================================================================================================================================
	void ecsComponentsKeyCompact::AddComponent( const uint32_t _componentIndex, const uint16_t _chunckIndex, const uint16_t _elementIndex )
	{
		assert( m_nextElement < s_maxComponentsPerEntity );
		assert( GetIndex( _componentIndex ) == s_emptyKeyValue );

		chunck[m_nextElement] = _chunckIndex;
		element[m_nextElement] = _elementIndex;

		SetIndex( _componentIndex, m_nextElement );

		++m_nextElement;
	}

	//================================================================================================================================
	// Removes a component 
	// O(n) if removing an arbitrary component
	// O(1) if removing the last component	
	//  ( n = ecsComponents::count )
	//================================================================================================================================
	void ecsComponentsKeyCompact::RemoveComponent( const uint32_t _removedecsComponentIndex )
	{
		assert( ! IsEmpty() );
		const uint32_t lastecsComponentIndex = m_nextElement - 1;
		const uint32_t removedElementIndex = GetIndex( _removedecsComponentIndex );

		assert( removedElementIndex != s_emptyKeyValue );

		// Swap if component is not at the end
		if ( removedElementIndex != lastecsComponentIndex )
		{
			
			// Finds the component index referencing the last component key
			uint32_t swappedecsComponentIndex = 0;
			uint32_t swappedIndex = 0;
			for ( swappedecsComponentIndex = 0; swappedecsComponentIndex < ecsComponents::count; ++ swappedecsComponentIndex) {
				swappedIndex = GetIndex( swappedecsComponentIndex );
				if ( swappedIndex == lastecsComponentIndex ){ break; }
			}

			// Swap
			chunck [removedElementIndex] =	chunck[swappedIndex];
			element[removedElementIndex] =	element[swappedIndex];
			SetIndex( swappedecsComponentIndex, removedElementIndex );			
		} 

		// Clear
		-- m_nextElement;
		SetIndex( _removedecsComponentIndex, s_emptyKeyValue );
		chunck[m_nextElement] = 0;
		element[m_nextElement] = 0;
	}
}