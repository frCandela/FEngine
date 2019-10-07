#pragma once

#include "core/ecs/fanECSConfig.h"

namespace fan {
	class EntityData {
	public:
		EntityData() {
			m_bitset[aliveBit] = 1;
		}

		template< typename _componentType > void AddComponent();
		template< typename _componentType > void RemoveComponent();
		template< typename _tagType >		void AddTag();
		template< typename _tagType >		void RemoveTag();
		template< typename _signatureType > void SetSignature();
		inline Bitset GetBitset() const { return m_bitset; }

		void Kill() { m_bitset[aliveBit] = 0; }
		bool IsAlive() const { return m_bitset[aliveBit]; }


	private:
		uint32_t components[Components::count];
		Bitset   m_bitset; // signature bitset
	};
	static_assert( sizeof(EntityData) == sizeof( Bitset ) + Components::count * sizeof( uint32_t ) ); // Beware of paaaaaaading :')

	//================================================================================================================================
	//================================================================================================================================
	template< typename _componentType > void EntityData::AddComponent() {
		static_assert( IsComponent<_componentType>::value );
		m_bitset[ meta::Find::List< _componentType, Components>::value ] = 1;
	}
	
	//================================================================================================================================
	//================================================================================================================================
	template< typename _componentType > void EntityData::RemoveComponent() {
		static_assert( IsComponent<_componentType>::value );
		m_bitset[meta::Find::List< _componentType, Components>::value] = 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _tagType > void EntityData::AddTag() {
		static_assert( IsTag< _tagType > );
		m_bitset[meta::Find::List< _componentType, Components>::value + Tags::count] = 1;
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _tagType > void EntityData::RemoveTag() {
		static_assert( IsTag< _tagType > );
		m_bitset[meta::Find::List< _componentType, Components>::value + Tags::count ] = 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _signatureType > void EntityData::SetSignature() {
		m_bitset = _signatureType;
	}
}