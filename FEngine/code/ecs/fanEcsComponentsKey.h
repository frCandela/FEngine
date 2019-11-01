#pragma once

#include "ecs/fanECSConfig.h"
#include "core/meta/fanMetaMath.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	struct ecsComponentsKey
	{
	public:
		static constexpr size_t maxComponentsPerEntity = ecsComponents::count;

		ecsComponentsKey() {	bitset[aliveBit] = 1; }

		uint16_t	chunck[ecsComponents::count];		// chunck of each components
		uint16_t	element[ecsComponents::count];	// index of each components
		ecsBitset   bitset;								// signature

		void Kill() { bitset[aliveBit] = 0; }
		bool IsAlive() const { return   bitset[aliveBit]; }
		bool IsDead() const { return  ! bitset[aliveBit]; }
	};

	//================================================================================================================================
	// Limits the maximum number of components on every entity
	// Better memory efficiency when there are many ecsComponents types
	// Slower access time than the simple implementation 
	// TODO measure average access time 
	//================================================================================================================================
	struct ecsComponentsKeyCompact
	{
	public:		
		static constexpr size_t s_indexWidth = 3;
		static constexpr size_t s_maxComponentsPerEntity = S_Pow( 2, s_indexWidth ) - 1;
		static constexpr size_t s_clearMask = ( 1 << s_indexWidth ) - 1;
		
		uint16_t									chunck [s_maxComponentsPerEntity];	// chunck of each component
		uint16_t									element[s_maxComponentsPerEntity];	// index of each component
		ecsBitset									bitset;								// signature
		
		ecsComponentsKeyCompact();

		uint32_t GetIndex( const uint32_t _index ) const;
		void	 SetKey( const uint32_t _index, const uint32_t _value );
		void	 SetComponent( const uint32_t _index, const uint16_t _chunckIndex, const uint16_t _elementIndex );
		void	 Reset();

		void Kill() { bitset[aliveBit] = 0; }
		bool IsAlive() const { return   bitset[aliveBit]; }
		bool IsDead() const { return  !bitset[aliveBit]; }

	//private:
		using indicesBitset = Bitset2::bitset2< s_indexWidth * ecsComponents::count >;

		indicesBitset m_componentsKeys;		// Index of the components in the element&chunck arrays
		int			  m_nextElement = 0;	// Next available element in the element&chunck arrays
	};


	static constexpr size_t totosize = sizeof( ecsComponentsKey );
	static constexpr size_t totosize2 = sizeof( ecsComponentsKeyCompact );
}