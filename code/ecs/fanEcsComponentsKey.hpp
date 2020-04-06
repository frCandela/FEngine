#pragma once

#include "scene/fanScenePrecompiled.hpp"

namespace fan
{
	// @old 04/01/2020 taken from the old static ecs ( rip)
	// implements a memory efficient storage of components for ecs entities
	// can be usefull if the ecs entities consumes too much memory

// 	//================================================================================================================================
// 	// Bitset part of a componentsKey
// 	//================================================================================================================================
// 	class ecsComponentsKeyCommon
// 	{
// 	public:
// 		ecsComponentsKeyCommon() { m_bitset[ aliveBit ] = 1; }
// 
// 		inline void Kill() { m_bitset[ aliveBit ] = 0; }
// 		inline bool IsAlive() const { return   m_bitset[ aliveBit ]; }
// 		inline bool IsDead() const { return  !m_bitset[ aliveBit ]; }
// 		inline bool HasComponent( const int _componentID ) const { return m_bitset[ _componentID ]; }
// 		inline bool HasTag( const int _tagID ) const { return m_bitset[ _tagID ]; }
// 		inline void SetTag( const int _tagID, const bool _value ) { m_bitset[ _tagID ] = _value; }
// 		inline bool MatchSignature( const ecsBitset _bitset ) const { return  ( m_bitset & _bitset ) == _bitset; }
// 		inline const ecsBitset& GetBitset() const { return m_bitset; }
// 
// 	protected:
// 		ecsBitset		m_bitset;						// signature
// 	};
// 
// 	//================================================================================================================================
// 	// Limits the maximum number of components on every entity
// 	// Better memory efficiency when there are many ecsComponents types
// 	// Slower access time than the simple implementation 
// 	// TODO measure average access time 
// 	//================================================================================================================================
// 	class ecsComponentsKeyCompact : public ecsComponentsKeyCommon
// 	{
// 	public:
// 		static constexpr size_t s_indexWidth = 4;
// 		static constexpr size_t s_maxComponentsPerEntity = S_Pow( 2, s_indexWidth ) - 1;
// 		static constexpr size_t s_emptyKeyValue = ( 1 << s_indexWidth ) - 1;
// 		using indicesBitset = Bitset2::bitset2< s_indexWidth * ecsComponents::count >;
// 
// 
// 		ecsComponentsKeyCompact();
// 
// 		const ecsComponentIndex&	GetIndex( const uint32_t _componentID ) const { return m_indices[ GetSubIndex( _componentID ) ]; }
// 		void						AddComponent( const uint32_t _componentID, const ecsComponentIndex& _index );
// 		ecsComponentIndex			RemoveComponent( const uint32_t _componentID );
// 
// 		void OnGui();
// 	private:
// 		ecsComponentIndex	m_indices[ s_maxComponentsPerEntity ];
// 		indicesBitset		m_componentsKeys;						// Index of the components in the element&chunck arrays
// 		uint32_t			m_nextElement = 0;						// Next available element in the element&chunck arrays
// 
// 		uint32_t					GetSubIndex( const uint32_t _componentIndex ) const;
// 		void						SetSubIndex( const uint32_t _componentIndex, const uint32_t _value );
// 		void						Reset();
// 		const ecsComponentIndex*    Data() const { return m_indices; }
// 		uint32_t					Count() const { return m_nextElement; }
// 		uint32_t					IsEmpty() const { return m_nextElement == 0; }
// 	};
// 
// 
// 
// 	using ecsComponentsKey = ecsComponentsKeyCompact; //ecsComponentsKeyFast;
// 	//static constexpr size_t totosize = sizeof( ecsComponentsKeyFast );
// 	//static constexpr size_t totosize2 = sizeof( ecsComponentsKeyCompact );
}