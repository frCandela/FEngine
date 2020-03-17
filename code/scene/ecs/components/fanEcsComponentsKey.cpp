//#include "scene/ecs/fanEcsComponentsKey.hpp"

namespace fan
{
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	ecsComponentsKeyCompact::ecsComponentsKeyCompact()
// 	{
// 		Reset();
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void ecsComponentsKeyCompact::Reset()
// 	{
// 		for ( int elemIndex = 0; elemIndex < s_maxComponentsPerEntity; elemIndex++ )
// 		{
// 			m_indices[ elemIndex ].chunckIndex = 0;
// 			m_indices[ elemIndex ].componentIndex = 0;
// 		}
// 
// 		m_componentsKeys = ~indicesBitset( 0 );
// 		m_nextElement = 0;
// 	}
// 
// 	//================================================================================================================================
// 	// Returns the index of a component "_index" in the chunck&element arrays
// 	//================================================================================================================================
// 	uint32_t ecsComponentsKeyCompact::GetSubIndex( const uint32_t _componentIndex ) const
// 	{
// 		return  ( uint32_t ) ( ( m_componentsKeys >> ( _componentIndex * s_indexWidth ) )& indicesBitset( s_emptyKeyValue ) ).data()[ 0 ];
// 	}
// 
// 	//================================================================================================================================
// 	// Set the key at index "_index" to point to a specific "_value" which is an index in the chunck&element arrays
// 	//================================================================================================================================
// 	void ecsComponentsKeyCompact::SetSubIndex( const uint32_t _componentIndex, const uint32_t _value )
// 	{
// 		indicesBitset valueMask = indicesBitset( _value ) << ( _componentIndex * s_indexWidth );
// 		indicesBitset clearMask = ~( indicesBitset( s_emptyKeyValue ) << ( _componentIndex * s_indexWidth ) );
// 
// 		m_componentsKeys &= clearMask;
// 		m_componentsKeys |= valueMask;
// 	}
// 
// 	//================================================================================================================================
// 	// Sets the chunck&element indices for the component at index "_index"
// 	// Makes the key at index "_index" point towards a new component index in the chunck&element arrays
// 	//================================================================================================================================
// 	void ecsComponentsKeyCompact::AddComponent( const uint32_t _componentID, const ecsComponentIndex& _index )
// 	{
// 		assert( m_nextElement < s_maxComponentsPerEntity );
// 		assert( GetSubIndex( _componentID ) == s_emptyKeyValue );
// 		assert( m_bitset[ _componentID ] == 0 );
// 
// 		m_indices[ m_nextElement ] = _index;
// 
// 		SetSubIndex( _componentID, m_nextElement );
// 
// 		m_bitset[ _componentID ] = 1;
// 
// 		++m_nextElement;
// 	}
// 
// 	//================================================================================================================================
// 	// Removes a component 
// 	//================================================================================================================================
// 	ecsComponentIndex ecsComponentsKeyCompact::RemoveComponent( const uint32_t _componentID )
// 	{
// 		assert( !IsEmpty() );
// 		const uint32_t lastecsComponentIndex = m_nextElement - 1;
// 		const uint32_t removedElementIndex = GetSubIndex( _componentID );
// 		ecsComponentIndex removedIndexCpy = m_indices[ removedElementIndex ];
// 
// 		assert( removedElementIndex != s_emptyKeyValue );
// 		m_bitset[ _componentID ] = 0;
// 
// 		// Swap if component is not at the end
// 		if ( removedElementIndex != lastecsComponentIndex )
// 		{
// 			// Finds the component index referencing the last component key
// 			uint32_t swappedComponentIndex = 0;
// 			uint32_t swappedIndex = 0;
// 			for ( swappedComponentIndex = 0; swappedComponentIndex < ecsComponents::count; ++swappedComponentIndex )
// 			{
// 				swappedIndex = GetSubIndex( swappedComponentIndex );
// 				if ( swappedIndex == lastecsComponentIndex ) { break; }
// 			}
// 
// 			// Swap
// 			m_indices[ removedElementIndex ] = m_indices[ swappedIndex ];
// 			SetSubIndex( swappedComponentIndex, removedElementIndex );
// 		}
// 
// 		// Clear
// 		--m_nextElement;
// 		SetSubIndex( _componentID, s_emptyKeyValue );
// 		m_indices[ m_nextElement ].chunckIndex = 0;
// 		m_indices[ m_nextElement ].componentIndex = 0;
// 
// 		return removedIndexCpy;
// 	}
// 
// 	//================================================================================================================================
// 	// For testing
// 	//================================================================================================================================
// 	void ecsComponentsKeyCompact::OnGui()
// 	{
// 		if ( ImGui::Begin( "ecsComponentsKeyCompact test" ) )
// 		{
// 			if ( ImGui::Button( "reset" ) ) { Reset(); }
// 
// 			static int elemIndex = 0; static int chunckIndex = 0; static int componentIndex = 0;
// 			ImGui::DragInt( "componentIndex", &componentIndex, 1, 0, ecsComponents::count - 1 );
// 			ImGui::DragInt( "elemIndex", &elemIndex );
// 			ImGui::DragInt( "chunckIndex", &chunckIndex );
// 			if ( ImGui::Button( "Add" ) )
// 			{
// 				AddComponent( ( uint32_t ) componentIndex, { ( uint16_t ) chunckIndex, ( uint16_t ) elemIndex } );
// 			}
// 			if ( ImGui::Button( "Remove" ) )
// 			{
// 				RemoveComponent( ( uint32_t ) componentIndex );
// 			}
// 
// 			ImGui::Separator();
// 
// 			int tmpNext = m_nextElement;
// 			ImGui::DragInt( "lastElement", &tmpNext );
// 
// 			// key
// 			{
// 				std::stringstream ss;
// 				for ( int bitIndex = int( m_componentsKeys.size() ) - 1; bitIndex >= 0; --bitIndex )
// 				{
// 					ss << m_componentsKeys[ bitIndex ];
// 					if ( bitIndex % s_indexWidth == 0 )
// 					{
// 						ss << " ";
// 					}
// 				}
// 				ImGui::Text( ss.str().c_str() );
// 			}
// 
// 			// clear mask
// 			{
// 				ecsComponentsKeyCompact::indicesBitset clearMask = ~( ecsComponentsKeyCompact::indicesBitset( ecsComponentsKeyCompact::s_emptyKeyValue ) << ( componentIndex * ecsComponentsKeyCompact::s_indexWidth ) );
// 				std::stringstream ss;
// 				for ( int bitIndex = int( m_componentsKeys.size() ) - 1; bitIndex >= 0; --bitIndex )
// 				{
// 					ss << clearMask[ bitIndex ];
// 					if ( bitIndex % s_indexWidth == 0 )
// 					{
// 						ss << " ";
// 					}
// 				}
// 				ss << "clear mask";
// 				ImGui::Text( ss.str().c_str() );
// 			}
// 
// 			// key base 10
// 			{
// 				std::stringstream ss;
// 				for ( int i = ecsComponents::count - 1; i >= 0; --i )
// 				{
// 					ss << "  " << GetSubIndex( i ) << " ";
// 				}
// 				ImGui::Text( ss.str().c_str() );
// 			}
// 
// 			// elements
// 			{
// 				std::stringstream ss;
// 				for ( int i = 0; i < ecsComponentsKeyCompact::s_maxComponentsPerEntity; ++i )
// 				{
// 
// 					ss << m_indices[ i ].chunckIndex << "-" << m_indices[ i ].componentIndex << "  ";
// 				}
// 				ImGui::Text( ss.str().c_str() );
// 			}
// 		}
// 		ImGui::End();
// 	}

}