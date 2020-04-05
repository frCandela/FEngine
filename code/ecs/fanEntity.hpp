#pragma once

#include "fanEcsTypes.hpp"
#include "fanComponent.hpp"

namespace fan 
{
	//==============================================================================================================================================================
	// An Entity is a data struct that is composed of a signature and pointers to components
	// - signature is the bitset of the components referenced by the entity
	// - handle is a unique key that allows access to an entity,( 0 == invalid )
	//==============================================================================================================================================================
	struct Entity
	{
		static constexpr int s_maxComponentsPerEntity = 29;
		Component* components[s_maxComponentsPerEntity];
		int componentCount = 0;
		Signature signature;
		EntityHandle handle = 0;
		
		bool HasTag( const ComponentIndex _index ) const		{ return signature[_index] == 1;		}
		bool HasComponent( const ComponentIndex _index ) const	{ return signature[_index] == 1;		}
		bool IsAlive() const									{ return  signature[ecAliveBit] == 1;	}
		void Kill()												{ signature[ecAliveBit] = 0;			}
	};
	static constexpr size_t sizeEntity = sizeof( Entity );
	static_assert( sizeEntity == 256 );
}