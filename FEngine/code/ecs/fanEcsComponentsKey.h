#pragma once

#include "ecs/fanECSConfig.h"
#include "core/meta/fanMetaMath.h"

namespace fan {

	//================================================================================================================================
	// Used to access a component data inside the ecsComponentTuple
	//================================================================================================================================
	struct ecsComponentIndex
	{
		using chunckType = uint16_t;
		using componentType = uint16_t;

		chunckType		chunckIndex;
		componentType	componentIndex;
	};

	//================================================================================================================================
	// Bitset part of a componentsKey
	//================================================================================================================================
	class ecsComponentsKeyCommon
	{
	public:
		ecsComponentsKeyCommon() { m_bitset[aliveBit] = 1; }
		
		inline void Kill() { m_bitset[aliveBit] = 0; }
		inline bool IsAlive() const { return   m_bitset[aliveBit]; }
		inline bool IsDead() const { return  !m_bitset[aliveBit]; }
		inline bool HasComponent( const int _componentID ) const { return m_bitset[_componentID]; }
		inline bool HasTag( const int _tagID ) const		 { return m_bitset[_tagID]; }
		inline void SetTag( const int _tagID, const bool _value ) { m_bitset[_tagID] = _value; }
		inline bool MatchSignature( const ecsBitset _bitset ) const { return  ( m_bitset & _bitset ) == _bitset; }
		inline const ecsBitset& GetBitset() const { return m_bitset; }

	protected:
		ecsBitset		m_bitset;						// signature
	};

	//================================================================================================================================

	//================================================================================================================================
	class ecsComponentsKey : public ecsComponentsKeyCommon
	{
	public:
 		const ecsComponentIndex& GetIndex( const uint32_t _componentID ) const { return m_indices[_componentID]; }
		void AddComponent( const uint32_t _componentID, const ecsComponentIndex& _index ){
			assert( m_bitset[_componentID] == 0 ); // entity already has _componentType
			m_indices[_componentID] = _index; 
			m_bitset[_componentID] = 1;
		}
		ecsComponentIndex RemoveComponent( const uint32_t _componentID ) { 
			m_bitset[_componentID] = 0;
			return m_indices[_componentID]; 
		}

	private:
		ecsComponentIndex	m_indices[ecsComponents::count];// index of each components
	};

	//================================================================================================================================
	// Limits the maximum number of components on every entity
	// Better memory efficiency when there are many ecsComponents types
	// Slower access time than the simple implementation 
	// TODO measure average access time 
	//================================================================================================================================
	class ecsComponentsKeyCompact : public ecsComponentsKeyCommon
	{
	public:		
		ecsComponentsKeyCompact();

		static constexpr size_t s_indexWidth = 4;
		static constexpr size_t s_maxComponentsPerEntity = S_Pow( 2, s_indexWidth ) - 1;
		static constexpr size_t s_emptyKeyValue = ( 1 << s_indexWidth ) - 1;
		
		uint16_t	chunck [s_maxComponentsPerEntity];	// chunck of each component
		uint16_t	element[s_maxComponentsPerEntity];	// index of each component

		void AddComponent( const uint32_t _componentIndex, const uint16_t _chunckIndex, const uint16_t _elementIndex );
		void RemoveComponent( const uint32_t _removedecsComponentIndex );
		void Reset();

		uint32_t Count() const   { return m_nextElement; }
		uint32_t IsEmpty() const { return m_nextElement == 0; }

	//private:
		using indicesBitset = Bitset2::bitset2< s_indexWidth * ecsComponents::count >;

		indicesBitset m_componentsKeys;		// Index of the components in the element&chunck arrays
		uint32_t	  m_nextElement = 0;	// Next available element in the element&chunck arrays

		uint32_t GetIndex( const uint32_t _componentIndex ) const;
		void	 SetIndex( const uint32_t _componentIndex, const uint32_t _value );

	};


	static constexpr size_t totosize = sizeof( ecsComponentsKey );
	static constexpr size_t totosize2 = sizeof( ecsComponentsKeyCompact );
}